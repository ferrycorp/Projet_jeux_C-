#include "player.h"
#include <QGraphicsScene>
#include <QDir>

Player::Player(QGraphicsItem* parent)
        : QGraphicsPixmapItem(parent), currentFrame(0) {

    step = 2;
    rotationAngle = 0;
    for (int i = 1; i <= 6; ++i) {
        walkFrames.append(QPixmap(QString("../images/perso_%1.png").arg(i)).scaled(32, 32));
    }

    setPixmap(walkFrames[0]);
    setFlag(QGraphicsItem::ItemIsFocusable);
    setFocus();

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Player::nextFrame);
    animationTimer->setInterval(150);

    shootCooldownTimer = new QTimer(this);
    shootCooldownTimer->setSingleShot(true);

    connect(shootCooldownTimer, &QTimer::timeout, [this]() {
        canShoot = true;
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