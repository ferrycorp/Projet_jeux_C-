#include "mob.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QtMath>
#include <QDebug>


Enemy::Enemy(Player* player, int tileSize, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent), targetPlayer(player) {


    setPixmap(QPixmap("../images/enemy.png").scaled(32, 32));
    speed = 1;

    // Position aléatoire entre 0 et 10 tuiles
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
        // Facultatif : repositionne ou supprime l'ennemi après impact
        scene()->removeItem(this);
        deleteLater();
    }
}