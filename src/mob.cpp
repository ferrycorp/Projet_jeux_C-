#include "mob.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>


Enemy::Enemy(Player* player, int tileSize, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent), targetPlayer(player) {

    for (int i = 1; i <= 4; ++i) {
        QString path = QString("../images/enemy_%1.png").arg(i);
        QPixmap frame(path);
        walkFrames.append(frame.scaled(32, 32));
    }

    setPixmap(walkFrames[0]);

    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, [this]() {
        currentFrame = (currentFrame + 1) % walkFrames.size();
        QPixmap base = walkFrames[currentFrame];

        // Vérifie la position du joueur pour choisir la direction
        if (targetPlayer && targetPlayer->x() < this->x()) {
            // Joueur à gauche → image normale
            setPixmap(base);
        } else {
            // Joueur à droite → retourner l'image horizontalement
            QPixmap flipped = base.transformed(QTransform().scale(-1, 1));
            setPixmap(flipped);
        }
    });
    animationTimer->start(150); // Vitesse de l'animation

    speed = 1;

    int tileX = QRandomGenerator::global()->bounded(16);
    int tileY = QRandomGenerator::global()->bounded(16);
    setPos(tileX * tileSize, tileY * tileSize);

    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Enemy::moveTowardPlayer);
    moveTimer->start(100);
}

void Enemy::moveTowardPlayer() {
    if (!targetPlayer) return;

    QPointF enemyPos = pos();
    QPointF playerPos = targetPlayer->pos();

    // Calcul du vecteur direction
    qreal dx = playerPos.x() - enemyPos.x();
    qreal dy = playerPos.y() - enemyPos.y();
    qreal length = std::sqrt(dx*dx + dy*dy);

    if (length > 1e-2) {
        dx = speed * dx / length;
        dy = speed * dy / length;

        setPos(enemyPos.x() + dx, enemyPos.y() + dy);
    }

    checkCollisionWithPlayer();
}

void Enemy::checkCollisionWithPlayer() {
    if (!targetPlayer)
        return;

    if (collidesWithItem(targetPlayer)) {
        targetPlayer->takeDamage(10); // retire 10 points
        scene()->removeItem(this);
        deleteLater();
    }
}

void Enemy::takeDamage(int amount) {
    health -= amount;

    if (health <= 0) {
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
    }
}