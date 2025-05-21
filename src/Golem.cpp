#include "Golem.h"


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
          attackCounter(0)
{
    // Initialisation des timers
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Golem::updateAnimation);
    animationTimer->start(ANIM_FRAME_TIME);

    actionTimer = new QTimer(this);
    connect(actionTimer, &QTimer::timeout, this, &Golem::performAction);
    actionTimer->start(ACTION_INTERVAL);

    projectileTimer = new QTimer(this);
    connect(projectileTimer, &QTimer::timeout, this, &Golem::fireProjectile);

    chargeTimer = new QTimer(this);
    chargeTimer->setSingleShot(true);
    connect(chargeTimer, &QTimer::timeout, this, &Golem::performChargeAttack);

    damageTimer = new QTimer(this);
    damageTimer->setSingleShot(true);
    connect(damageTimer, &QTimer::timeout, this, &Golem::flashDamage);

    collisionTimer = new QTimer(this);
    connect(collisionTimer, &QTimer::timeout, this, &Golem::checkCollisions);
    collisionTimer->start(50);  // Vérification fréquente des collisions

    // Chargement des animations depuis la sprite sheet
    loadAnimations();

    // Définir le pivot au centre du sprite pour les rotations
    setTransformOriginPoint(spriteSize / 2, spriteSize / 2);

    // Définir la hitbox (zone de collision)
    setOffset(-spriteSize / 2, -spriteSize / 2);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setZValue(2);
    setShapeMode(QGraphicsPixmapItem::BoundingRectShape); // pour simplifier les collisions
}

Golem::~Golem() {
    // Nettoyage des timers
    animationTimer->stop();
    actionTimer->stop();
    projectileTimer->stop();
    chargeTimer->stop();
    damageTimer->stop();
    collisionTimer->stop();

    delete animationTimer;
    delete actionTimer;
    delete projectileTimer;
    delete chargeTimer;
    delete damageTimer;
    delete collisionTimer;
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
        }
    }

    return frames;
}

void Golem::loadAnimations() {
    // Chargement de la sprite sheet
    idleFrames = loadAnimationFrames("idle", "idle_golem", 4);  // charge 10 frames d'idle
    //walkFrames = loadAnimationFrames("walk", "walk_golem", 8);   // charge 8 frames de marche
   // attackFrames = loadAnimationFrames("attack", "attack_golem", 6); // etc.

    if (idleFrames.isEmpty())
        qDebug() << "Erreur : frames 'idle' non chargées";
}




void Golem::updateAnimation() {
    if (animationFrames.isEmpty()) return;

    currentFrame = (currentFrame + 1) % animationFrames.size();
    setPixmap(animationFrames[currentFrame]);
}

void Golem::changeState(GolemState newState) {
    if (currentState == newState) return;

    currentState = newState;
    currentFrame = 0;

    // Définir la ligne d'animation en fonction de l'état
    switch (newState) {
        case IDLE:
            animationFrames = idleFrames;
            animationRow = 0;
            speed = BASE_SPEED;
            break;
        case WALKING:
            animationRow = 1; // Deuxième ligne = marche
            speed = BASE_SPEED;
            break;
        case FIRING:
            animationRow = 2; // Troisième ligne = tir
            break;
        case CHARGING:
            animationRow = 3; // Quatrième ligne = charge
            speed = CHARGE_SPEED;
            break;
        case HIT:
            animationRow = 4; // Cinquième ligne = touché
            break;
        case DYING:
            animationRow = 5; // Sixième ligne = mort
            animationTimer->setInterval(150); // Animation plus lente pour la mort
            actionTimer->stop();
            projectileTimer->stop();
            chargeTimer->stop();
            collisionTimer->stop();
            break;
        default:
            animationFrames = idleFrames; // Valeur par défaut
            break;
    }

    // Vérifier que l'animation existe
    if (animationRow >= animations.size()) {
        animationRow = 0;
    }
}

void Golem::performAction() {
    if (currentState == DYING) return;

    if (!player) return;

    // Calculer la distance au joueur
    QPointF playerPos = player->pos();
    QPointF bossPos = pos();
    QPointF diff = playerPos - bossPos;
    double distance = std::sqrt(diff.x() * diff.x() + diff.y() * diff.y());

    // Décider de l'action en fonction de la distance et d'autres facteurs
    if (attackCooldown > 0) {
        attackCooldown -= ACTION_INTERVAL;
    }

    if (currentState == CHARGING) {
        // Déjà en train de charger, ne rien faire de plus
        return;
    }

    if (distance > DETECTION_RANGE) {
        // Joueur hors de portée, passer en mode IDLE
        changeState(IDLE);
        return;
    }

    // Orienter le boss vers le joueur
    rotateTowardsPlayer();

    if (distance <= ATTACK_RANGE && attackCooldown <= 0) {
        // Joueur à portée d'attaque
        int attackChoice = std::rand() % 10; // Utiliser std::rand() à la place de QRandomGenerator

        if (attackChoice < 3 && !projectileTimer->isActive()) {
            // 30% de chance de tirer
            changeState(FIRING);
            projectileTimer->start(500); // Commencer à tirer après 500ms
            attackCooldown = PROJECTILE_COOLDOWN;
        }
        else if (attackChoice < 6 && !chargeTimer->isActive()) {
            // 30% de chance de charger
            startChargeAttack();
            attackCooldown = CHARGE_COOLDOWN;
        }
        else {
            // 40% de chance de continuer à se déplacer
            changeState(WALKING);
            moveTowardsPlayer(speed);
        }
    }
    else {
        // Joueur trop loin pour attaquer, se rapprocher
        changeState(WALKING);
        moveTowardsPlayer(speed);
    }
}


void Golem::fireProjectile() {
    if (!player || !scene()) return;

    // Créer un projectile
    QPointF bossPos = pos();
    QPointF playerPos = player->pos();
    QPointF diff = playerPos - bossPos;

    // Calculer l'angle vers le joueur
    double angle = std::atan2(diff.y(), diff.x()) * 180 / M_PI;

    // Créer le projectile (ajustez selon votre classe Projectile)
    Projectile* proj = new Projectile(angle, spriteSize/2);
    proj->setPos(bossPos);
    proj->setData(0, "boss_projectile"); // Pour identifier ce projectile comme venant du boss

    scene()->addItem(proj);

    // Arrêter le tir après 3 projectiles
    static int shotCount = 0;
    shotCount++;

    if (shotCount >= 3) {
        shotCount = 0;
        projectileTimer->stop();
        changeState(WALKING);
    }
}



void Golem::moveTowardsPlayer(double moveSpeed) {
    if (!player || !scene()) return;

    QPointF moveVector = calculateMovementVector(moveSpeed);
    QPointF newPos = pos() + moveVector;

    // Vérifier les limites de la scène
    QRectF bounds = scene()->sceneRect();
    if (bounds.contains(QRectF(newPos, QSizeF(spriteSize, spriteSize)))) {
        setPos(newPos);
    }
}

QPointF Golem::calculateMovementVector(double moveSpeed) {
    if (!player) return QPointF(0, 0);

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

void Golem::rotateTowardsPlayer() {
    if (!player) return;

    QPointF playerPos = player->pos();
    QPointF bossPos = pos();
    QPointF diff = playerPos - bossPos;

    // Calculer l'angle en degrés
    double angle = std::atan2(diff.y(), diff.x()) * 180 / M_PI;

    // Appliquer la rotation au sprite
    setRotation(angle);
}

void Golem::takeDamage(int amount) {
    if (!canTakeDamage || currentState == DYING) return;

    health -= amount;
    qDebug() << "Boss prend" << amount << "dégâts. HP restant:" << health;

    // Clignotement rouge
    isFlashing = true;
    canTakeDamage = false;
    damageTimer->start(200);

    if (health <= 0) {
        // Boss vaincu
        changeState(DYING);
        // Émettre le signal de défaite du boss
        emit golemDefeated();

        // Désactiver les collisions
        setEnabled(false);

        // Générer des récompenses
        dropLoot();
    }
    else {
        // Changer brièvement l'état à HIT
        GolemState previousState = currentState;
        changeState(HIT);

        // Revenir à l'état précédent après un court délai
        QTimer::singleShot(300, this, [this, previousState]() {
            if (currentState == HIT) {
                changeState(previousState);
            }
        });
    }
}

void Golem::flashDamage() {
    isFlashing = false;
    canTakeDamage = true;
    updateAnimation(); // Rafraîchir l'animation pour enlever l'effet rouge
}

void Golem::checkCollisions() {
    if (!scene() || currentState == DYING) return;

    QList<QGraphicsItem*> collidingItems = scene()->collidingItems(this);

    for (QGraphicsItem* item : collidingItems) {
        // Vérifier si l'item est un projectile du joueur
        if (item->data(0).toString() == "player_projectile") {
            takeDamage(10);
            scene()->removeItem(item);
            delete item;
        }

    }
}

void Golem::update() {
    // Cette méthode peut être appelée à chaque frame du jeu
    checkCollisions();
}

void Golem::dropLoot() {
    // Fonction à implémenter selon votre système de loot
    qDebug() << "Le boss a été vaincu et a laissé tomber du loot";

    // Exemple: créer un item de récompense
    // Item* reward = new Item("upgrade", pos());
    // scene()->addItem(reward);
}
