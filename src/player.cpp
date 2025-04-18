#include "player.h"
#include <QGraphicsScene>
#include <QDir>

Player::Player(QGraphicsItem* parent)
        : QGraphicsPixmapItem(parent), currentFrame(0) {

    step = 3;
    rotationAngle = 0;

    initWeaponInventory();
    loadWalkAnimation();
    setupAnimationTimer();
    setupShootCooldownTimer();
    setupLavaDamageTimer();
    setupDamageFlashTimer();

}
void Player::loadWalkAnimation() {
    for (int i = 1; i <= 6; ++i) {
        walkFrames.append(QPixmap(QString("../images/perso_%1.png").arg(i)).scaled(32, 32));
    }

    if (!walkFrames.isEmpty()) {
        setPixmap(walkFrames[0]);
    } else {
        qDebug() << "Aucune frame chargée !";
    }

    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();
}

void Player::setupAnimationTimer() {
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Player::nextFrame);
    animationTimer->setInterval(100);
}

void Player::setupShootCooldownTimer() {
    shootCooldownTimer = new QTimer(this);
    shootCooldownTimer->setSingleShot(true);

    connect(shootCooldownTimer, &QTimer::timeout, [this]() {
        canShoot = true;
    });
}

void Player::setupLavaDamageTimer() {
    lavaDamageTimer = new QTimer(this);
    lavaDamageTimer->setInterval(500); // 0.5 sec
    connect(lavaDamageTimer, &QTimer::timeout, this, [this]() {
        takeDamage(1);
    });
}

void Player::setupDamageFlashTimer() {
    damageFlashTimer = new QTimer(this);
    damageFlashTimer->setSingleShot(true);

    connect(damageFlashTimer, &QTimer::timeout, [this]() {
        setPixmap(walkFrames[currentFrame]);
    });
}


void Player::keyPressEvent(QKeyEvent* event) {
    QPointF nextPos = pos();
    bool moved = false;

    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Q:
            keyLeft = true;
            nextPos.rx() -= step;
            rotationAngle = 270;
            moved = true;
            direction = Left;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            keyRight = true;
            nextPos.rx() += step;
            rotationAngle = 90;
            moved = true;
            direction = Right;
            break;
        case Qt::Key_Up:
        case Qt::Key_Z:
            keyUp = true;
            nextPos.ry() -= step;
            rotationAngle = 0;
            moved = true;
            direction = Up;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            keyDown = true;
            nextPos.ry() += step;
            rotationAngle = 180;
            moved = true;
            direction = Down;
            break;
        case Qt::Key_Space:
            shoot(32);
            break;
        case Qt::Key_1:
            switchWeapon(1);
            break;
        case Qt::Key_2:
            switchWeapon(2);
            break;

    }

    if (moved && scene()) {
        QRectF bounds = scene()->sceneRect();
        if (bounds.contains(QRectF(nextPos, boundingRect().size()))) {
            setPos(nextPos);

            // Démarrer l'animation si elle est arrêtée
            if (!moving) {
                animationTimer->start();
                moving = true;
            }
        }
    }
}

void Player::keyReleaseEvent(QKeyEvent* event) {
    switch (event->key()) {
        case Qt::Key_Left:
        case Qt::Key_Q:
            keyLeft = false;
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            keyRight = false;
            break;
        case Qt::Key_Up:
        case Qt::Key_Z:
            keyUp = false;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            keyDown = false;
            break;
    }

    // Si aucune touche n'est maintenue → arrêter animation
    if (!keyLeft && !keyRight && !keyUp && !keyDown) {
        animationTimer->stop();
        moving = false;
        currentFrame = 0;

        QPixmap base = walkFrames[0];
        QTransform transform;
        transform.rotate(rotationAngle);
        QPixmap rotated = base.transformed(transform, Qt::SmoothTransformation);
        setPixmap(rotated);
    }
}


void Player::nextFrame() {
    currentFrame = (currentFrame + 1) % walkFrames.size();

    // Appliquer une rotation à l'image actuelle
    QPixmap base = walkFrames[currentFrame];
    QTransform transform;
    transform.rotate(rotationAngle);
    QPixmap rotated = base.transformed(transform, Qt::SmoothTransformation);

    setPixmap(rotated);
}

void Player::shoot(int tileSize) {
    if (!canShoot)
        return;

    canShoot = false;
    shootCooldownTimer->start(300); // 300 ms cooldown

    qreal angle = 0;
    switch (direction) {
        case Up:    angle = 180; break;
        case Down:  angle = 0;  break;
        case Left:  angle = -90; break;
        case Right: angle = 90;   break;
    }

    Projectile* proj = new Projectile(angle, tileSize);
    proj->setPos(pos().x() + tileSize / 2, pos().y() + tileSize / 2);

    if (scene()) {
        scene()->addItem(proj);
    }
}

void Player::takeDamage(int amount) {
    hp -= amount;
    qDebug() << "🔥 Le joueur prend" << amount << "dégâts. HP restant :" << hp;

    flashRed();
    if (hp <= 0) {
        emit gameOver(); // envoie un signal si tu veux arrêter le jeu
        qDebug() << "💀 Le joueur est mort.";
    }
}

void Player::startLavaDamage() {
    if (!lavaDamageTimer->isActive()) {
        lavaDamageTimer->start();
    }
}

void Player::stopLavaDamage() {
    if (lavaDamageTimer->isActive()) {
        lavaDamageTimer->stop();
    }
}

void Player::flashRed() {
    QPixmap redFrame = walkFrames[currentFrame];

    QImage img = redFrame.toImage();
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            QColor color = img.pixelColor(x, y);
            if (color.alpha() > 0) {
                color.setRed(255);
                img.setPixelColor(x, y, color);
            }
        }
    }

    QPixmap redPixmap = QPixmap::fromImage(img);
    setPixmap(redPixmap);

    damageFlashTimer->start(150);  // clignote pendant 150 ms
}

void Player::setWeapon(Weapon* weapon) {
    currentWeapon = weapon;
}

void Player::switchWeapon(int index) {
    if (weaponInventory.contains(index)) {
        setWeapon(weaponInventory[index]);
        qDebug() << "🔄 Arme sélectionnée :" << currentWeapon->getName();
    }
}

QString Player::getCurrentWeaponName() const {
    return currentWeapon ? currentWeapon->getName() : "Aucune";
}

void Player::initWeaponInventory() {
    // Vider d’éventuelles armes précédentes
    qDeleteAll(weaponInventory);
    weaponInventory.clear();

    // Ajout des armes au slot 1 et 2
    weaponInventory.insert(1, new Gun());
    weaponInventory.insert(2, new Bat());

    // Arme par défaut : slot 1
    setWeapon(weaponInventory.value(1));
}



