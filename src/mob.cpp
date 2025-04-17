#include "mob.h"
#include <QGraphicsScene>
#include <QRandomGenerator>
#include <QtMath>

Enemy::Enemy(Player* player, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent), targetPlayer(player) {

    // Sprite ou image de base
    setPixmap(QPixmap("../images/enemy.jpg").scaled(32, 32));

    // Vitesse de déplacement
    speed = 1;

    // Position aléatoire dans les limites de la scène
    if (scene()) {
        QRectF sceneRect = scene()->sceneRect();
        qreal x = QRandomGenerator::global()->bounded(sceneRect.width());
        qreal y = QRandomGenerator::global()->bounded(sceneRect.height());
        setPos(x, y);
    }

    // Timer de mouvement
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Enemy::moveTowardPlayer);
    moveTimer->start(100);  // toutes les 100ms
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
}