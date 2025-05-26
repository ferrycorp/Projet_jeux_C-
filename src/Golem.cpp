#include "Golem.h"
#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include <QVector>

Golem::Golem(Player* targetPlayer, int spriteSize, QGraphicsItem* parent)
        : QGraphicsPixmapItem(parent),
          player(targetPlayer),
          health(MAX_HEALTH),
          spriteSize(spriteSize),
          currentState(IDLE),
          speed(BASE_SPEED),
          currentFrame(0),
          animationRow(0),
          isFlashing(false),
          canTakeDamage(true),
          attackCooldown(0),
          attackCounter(0),
          isDying(false) {  // ← Ajout de l'initialisation

    // Initialisation des timers
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Golem::updateAnimation);
    animationTimer->start(ANIM_FRAME_TIME);

    actionTimer = new QTimer(this);
    connect(actionTimer, &QTimer::timeout, this, &Golem::performAction);
    actionTimer->start(ACTION_INTERVAL);

    projectileTimer = new QTimer(this);
    connect(projectileTimer, &QTimer::timeout, this, &Golem::fireProjectile);

    damageTimer = new QTimer(this);
    damageTimer->setSingleShot(true);
    connect(damageTimer, &QTimer::timeout, this, &Golem::flashDamage);

    collisionTimer = new QTimer(this);
    connect(collisionTimer, &QTimer::timeout, this, &Golem::checkCollisions);
    collisionTimer->start(16);  // ~60 FPS pour une détection fluide

    // Chargement des animations depuis la sprite sheet
    loadAnimations();
    if (!idleFrames.isEmpty()) {
        setPixmap(idleFrames[0]);
        setTransformationMode(Qt::SmoothTransformation);

        // Appliquer la mise à l'échelle pour adapter à spriteSize souhaité
        qreal scaleFactor = static_cast<qreal>(spriteSize) / idleFrames[0].width();
        setScale(scaleFactor);

        animationFrames = idleFrames;
    }

    // Configuration de la hitbox pour les collisions
    setupCollisionBox();

    setFlag(QGraphicsItem::ItemIsFocusable);
    setZValue(2);
}

Golem::~Golem() {
    // ✅ Nettoyage sécurisé des timers
    if (animationTimer) {
        animationTimer->stop();
        animationTimer->deleteLater();
    }
    if (actionTimer) {
        actionTimer->stop();
        actionTimer->deleteLater();
    }
    if (projectileTimer) {
        projectileTimer->stop();
        projectileTimer->deleteLater();
    }
    if (damageTimer) {
        damageTimer->stop();
        damageTimer->deleteLater();
    }
    if (collisionTimer) {
        collisionTimer->stop();
        collisionTimer->deleteLater();
    }

    qDebug() << "Golem détruit proprement";
}

void Golem::setupCollisionBox() {
    // Créer une hitbox plus précise (plus petite que le sprite complet)
    int hitboxSize = spriteSize * 0.7; // 70% de la taille du sprite
    int offset = (spriteSize - hitboxSize) / 2;

    hitboxRect = QRectF(offset, offset, hitboxSize, hitboxSize);
}

QRectF Golem::boundingRect() const {
    return QRectF(0, 0, spriteSize, spriteSize);
}

QPainterPath Golem::shape() const {
    QPainterPath path;
    // Utiliser la hitbox pour la détection de collision
    path.addEllipse(hitboxRect); // Forme circulaire pour des collisions plus naturelles
    return path;
}

void Golem::updateAnimation() {
    if (animationFrames.isEmpty() || isDying) return;

    if (currentState == DYING) {
        if (currentFrame < animationFrames.size()) {
            setPixmap(animationFrames[currentFrame]);
            currentFrame++;

            // Quand l'animation de mort est terminée
            if (currentFrame >= animationFrames.size()) {
                // ✅ Arrêter l'animation et marquer pour suppression
                animationTimer->stop();
                return;
            }
        }
        return; // Ne boucle pas l'animation de mort
    }

    currentFrame = (currentFrame + 1) % animationFrames.size();
    setPixmap(animationFrames[currentFrame]);
}

QVector<QPixmap> Golem::loadAnimationFrames(const QString& folder, const QString& baseName, int frameCount) {
    QVector<QPixmap> frames;

    for (int i = 1; i <= frameCount; ++i) {
        QString path = QString("../Mecha-stone Golem 0.1/%1/%2 (%3).png")
                .arg(folder)
                .arg(baseName)
                .arg(i);

        QPixmap frame(path);
        if (!frame.isNull()) {
            frames.append(frame);
        } else {
            qDebug() << "Erreur de chargement:" << path;
            // Créer un sprite de fallback si l'image n'existe pas
            QPixmap fallback(spriteSize, spriteSize);
            fallback.fill(Qt::blue);
            frames.append(fallback);
        }
    }

    return frames;
}

void Golem::fireProjectile() {
    if (!player || !scene() || isDying) return;

    // Position du centre du golem pour le spawn du projectile
    QPointF bossCenter = pos() + QPointF(spriteSize/2, spriteSize/2);
    QPointF playerCenter = player->pos() + QPointF(player->boundingRect().width()/2, player->boundingRect().height()/2);
    QPointF diff = playerCenter - bossCenter;

    // Calculer l'angle vers le joueur
    double angle = std::atan2(diff.y(), diff.x()) * 180 / M_PI;

    // Créer le projectile
    Projectile* proj = new Projectile(angle, spriteSize/2);

    // Positionner le projectile au centre du golem
    proj->setPos(bossCenter);
    proj->setData(0, "boss_projectile");
    proj->setZValue(1); // S'assurer qu'il est visible

    scene()->addItem(proj);

    // Arrêter le tir après 3 projectiles
    shotCount++;
    if (shotCount >= 3) {
        shotCount = 0;
        projectileTimer->stop();
        changeState(WALKING);
    }
}

void Golem::checkCollisions() {
    if (!scene() || isDying) return;

    // Vérification plus robuste des collisions
    QList<QGraphicsItem*> allItems = scene()->items();

    for (QGraphicsItem* item : allItems) {
        if (!item || item == this) continue;

        QVariant data = item->data(0);
        if (data.isValid() && data.toString() == "player_projectile") {
            // Calculer la distance entre le projectile et le centre du golem
            QPointF projectileCenter = item->pos() + QPointF(item->boundingRect().width()/2, item->boundingRect().height()/2);
            QPointF golemCenter = pos() + QPointF(spriteSize/2, spriteSize/2);
            QPointF diff = projectileCenter - golemCenter;
            double distance = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());

            // Rayon de collision
            double collisionRadius = (spriteSize * 0.6) / 2; // Un peu plus large pour être plus permissif

            if (distance <= collisionRadius) {
                handleProjectileCollision(item);
                break; // Une seule collision par frame
            }
        }
    }
}

void Golem::handleProjectileCollision(QGraphicsItem* projectile) {
    if (!projectile || !scene() || isDying) return;

    takeDamage(10);

    // Supprimer le projectile de manière sécurisée
    scene()->removeItem(projectile);
    //projectile->deleteLater();

    qDebug() << "Collision détectée ! Golem touché par projectile.";
}

void Golem::takeDamage(int amount) {
    if (!canTakeDamage || isDying) return;

    health -= amount;
    qDebug() << "Golem prend" << amount << "dégâts. HP restant:" << health;

    // Effet de clignotement rouge
    isFlashing = true;
    canTakeDamage = false;

    auto *effect = new QGraphicsColorizeEffect;
    effect->setColor(Qt::red);
    effect->setStrength(0.7);
    setGraphicsEffect(effect);
    damageTimer->start(200);

    if (health <= 0 && !isDying) {
        isDying = true;
        changeState(DYING);

        // ✅ Arrêter tous les timers AVANT d'émettre le signal
        if (animationTimer) animationTimer->stop();
        if (actionTimer) actionTimer->stop();
        if (projectileTimer) projectileTimer->stop();
        if (damageTimer) damageTimer->stop();
        if (collisionTimer) collisionTimer->stop();

        // ✅ Désactiver l'item pour éviter les interactions
        setEnabled(false);

        // ✅ Émettre le signal APRÈS avoir sécurisé l'objet
        emit golemDefeated();

        // ✅ Programme la destruction après l'animation de mort
        int animationDuration = deathFrames.size() * 150; // 150ms par frame
        if (animationDuration == 0) animationDuration = 1000;

        // ✅ Timer unique pour la destruction - pas de lambda avec [this]
        QTimer* destructionTimer = new QTimer();
        destructionTimer->setSingleShot(true);

        connect(destructionTimer, &QTimer::timeout, [this, destructionTimer]() {
            destructionTimer->deleteLater();
            if (scene()) {
                scene()->removeItem(this);
            }
            this->deleteLater();
        });

        destructionTimer->start(animationDuration);

        // ✅ Redémarrer uniquement l'animation de mort
        if (animationTimer) {
            disconnect(animationTimer, nullptr, this, nullptr);
            connect(animationTimer, &QTimer::timeout, this, &Golem::updateAnimation);
            animationTimer->start(150);
        }
    }
}

void Golem::flashDamage() {
    if (isDying) return; // ✅ Éviter les effets pendant la mort

    isFlashing = false;
    canTakeDamage = true;
    setGraphicsEffect(nullptr);
}

void Golem::moveTowardsPlayer(double moveSpeed) {
    if (!player || !scene() || isDying) return;

    QPointF moveVector = calculateMovementVector(moveSpeed);
    QPointF newPos = pos() + moveVector;

    // Vérifier les limites de la scène avec une marge
    QRectF bounds = scene()->sceneRect();
    QRectF golemRect(newPos, QSizeF(spriteSize, spriteSize));

    // Ajouter une marge pour éviter que le golem sorte complètement
    bounds.adjust(10, 10, -10, -10);

    if (bounds.contains(golemRect)) {
        setPos(newPos);
    }
}

void Golem::changeState(GolemState newState) {
    if (currentState == newState || isDying) return;

    currentState = newState;
    currentFrame = 0;

    switch (newState) {
        case IDLE:
            animationFrames = idleFrames;
            animationRow = 0;
            speed = BASE_SPEED;
            break;
        case WALKING:
            animationRow = 1;
            speed = BASE_SPEED;
            break;
        case FIRING:
            animationRow = 2;
            break;
        case CHARGING:
            animationRow = 3;
            speed = CHARGE_SPEED;
            break;
        case HIT:
            animationRow = 4;
            break;
        case DYING:
            animationFrames = deathFrames;
            currentFrame = 0;
            break;
        default:
            animationFrames = idleFrames;
            break;
    }
}

void Golem::loadAnimations() {
    idleFrames = loadAnimationFrames("idle", "idle_golem", 4);
    deathFrames = loadAnimationFrames("death", "death_golem", 14);
    qDebug() << "Frames de mort chargées :" << deathFrames.size();

    if (idleFrames.isEmpty()) {
        qDebug() << "Erreur : frames 'idle' non chargées - création de sprites de fallback";
        // Créer des sprites de fallback
        for (int i = 0; i < 4; ++i) {
            QPixmap fallback(spriteSize, spriteSize);
            fallback.fill(Qt::darkBlue);
            idleFrames.append(fallback);
        }
    }
}

void Golem::performAction() {
    if (isDying || !player) return; // ✅ Vérifier isDying en premier

    // Calculer la distance au joueur
    QPointF playerPos = player->pos();
    QPointF bossPos = pos();
    QPointF diff = playerPos - bossPos;
    double distance = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());

    if (attackCooldown > 0) {
        attackCooldown -= ACTION_INTERVAL;
    }

    if (currentState == CHARGING) {
        return;
    }

    if (distance > DETECTION_RANGE) {
        changeState(IDLE);
        return;
    }

    if (distance <= ATTACK_RANGE && attackCooldown <= 0) {
        int attackChoice = std::rand() % 10;

        if (attackChoice < 3 && !projectileTimer->isActive()) {
            changeState(FIRING);
            projectileTimer->start(500);
            attackCooldown = PROJECTILE_COOLDOWN;
        }
        else {
            changeState(WALKING);
            moveTowardsPlayer(speed);
        }
    }
    else {
        changeState(WALKING);
        moveTowardsPlayer(speed);
    }
}

QPointF Golem::calculateMovementVector(double moveSpeed) {
    if (!player || isDying) return QPointF(0, 0);

    QPointF playerPos = player->pos();
    QPointF bossPos = pos();
    QPointF diff = playerPos - bossPos;

    // Normaliser le vecteur
    double length = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());
    if (length > 0) {
        diff.rx() /= length;
        diff.ry() /= length;
    }

    return QPointF(diff.x() * moveSpeed, diff.y() * moveSpeed);
}

void Golem::dropLoot() {
    qDebug() << "Le golem a été vaincu";
    // Implémenter selon votre système de loot
}