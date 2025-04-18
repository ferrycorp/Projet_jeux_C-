#include "projectile.h"
#include "mob.h"
#include <QtMath>
#include <QGraphicsScene>
#include <QList>

Projectile::Projectile(qreal angleDeg, int tileSize, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent) {

    setPixmap(QPixmap("../images/projectile.png").scaled(tileSize / 2, tileSize / 2));
    speed = 5;

    // Convertir l'angle en vecteur de direction
    qreal angleRad = qDegreesToRadians(angleDeg);
    direction = QPointF(qSin(angleRad), qCos(angleRad));

    // Timer pour déplacement
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Projectile::moveForward);
    moveTimer->start(30); // Vitesse de rafraîchissement
}

void Projectile::moveForward() {
    setPos(pos() + direction * speed);

    // Supprimer le projectile s’il sort de la scène
    if (!scene()->sceneRect().contains(pos())) {
        scene()->removeItem(this);
        deleteLater();
        return;
    }

    // Détection de collision avec un ennemi
    QList<QGraphicsItem*> colliding = collidingItems();
    for (QGraphicsItem* item : colliding) {
        Enemy* enemy = dynamic_cast<Enemy*>(item);
        if (enemy) {
            scene()->removeItem(enemy);
            enemy->deleteLater();

            scene()->removeItem(this);
            deleteLater();
            return;
        }
    }
}