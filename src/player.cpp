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
            break;
        case Qt::Key_Right:
        case Qt::Key_D:
            keyRight = true;
            nextPos.rx() += step;
            rotationAngle = 90;
            moved = true;
            break;
        case Qt::Key_Up:
        case Qt::Key_Z:
            keyUp = true;
            nextPos.ry() -= step;
            rotationAngle = 0;
            moved = true;
            break;
        case Qt::Key_Down:
        case Qt::Key_S:
            keyDown = true;
            nextPos.ry() += step;
            rotationAngle = 180;
            moved = true;
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
        case Qt::Key_Space:
            shoot(32);
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
    Projectile* proj = new Projectile(rotationAngle, tileSize);
    proj->setPos(pos().x() + 16, pos().y() + 16); // Centrer le projectile sur le joueur
    if (scene()) {
        scene()->addItem(proj);
    }
}