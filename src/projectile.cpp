#include "projectile.h"
#include "mob.h"
#include <QTimer>
#include <QGraphicsScene>
#include <QtMath>
#include <QList>
#include <QDebug>

Projectile::Projectile(qreal angleDeg, int tileSize, QGraphicsItem* parent)
        : QGraphicsPixmapItem(parent) {

    // Charger et redimensionner l'image
    QPixmap pixmap("../images/projectile.png");
    if (pixmap.isNull()) {
        // Image de fallback si le fichier n'existe pas
        pixmap = QPixmap(tileSize/4, tileSize/4);
        pixmap.fill(Qt::red); // Projectile rouge temporaire
        qDebug() << "Image projectile non trouvée, utilisation du fallback";
    } else {
        pixmap = pixmap.scaled(tileSize/4, tileSize/4, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    setPixmap(pixmap);
    setZValue(1);
    setData(0, "player_projectile");

    speed = 6; // Vitesse raisonnable

    // Convertir l'angle en vecteur de direction
    qreal angleRad = qDegreesToRadians(angleDeg);
    direction = QPointF(qSin(angleRad), qCos(angleRad));

    // Timer pour déplacement
    moveTimer = new QTimer(this);
    connect(moveTimer, &QTimer::timeout, this, &Projectile::moveForward);
    moveTimer->start(16); // ~60 FPS

    // Durée de vie du projectile
    lifetimeTimer = new QTimer(this);
    lifetimeTimer->setSingleShot(true);
    connect(lifetimeTimer, &QTimer::timeout, this, &Projectile::destroySelf);
    lifetimeTimer->start(5000);
}

Projectile::~Projectile() {

}

void Projectile::moveForward() {
    if (!scene()) {
        destroySelf();
        return;
    }

    QPointF newPos = pos() + direction * speed;
    setPos(newPos);

    QRectF sceneRect = scene()->sceneRect();
    QRectF projectileRect = QRectF(newPos, boundingRect().size());

    if (!sceneRect.intersects(projectileRect)) {
        destroySelf();
        return;
    }

    checkCollisions();
}

void Projectile::checkCollisions() {
    if (!scene()) return;

    auto collidingItems = scene()->collidingItems(this, Qt::IntersectsItemShape);
    for (QGraphicsItem* item : collidingItems) {
        if (item == this) continue;

        // Collision avec un Enemy
        if (auto* enemy = dynamic_cast<Enemy*>(item)) {
            enemy->takeDamage(1);
            QTimer::singleShot(0, this, &Projectile::destroySelf);
            return;
        }

        // Cas Golem ou autre via data()

        }
    }


void Projectile::destroySelf() {
    if (scene()) {
        scene()->removeItem(this);
    }
    deleteLater();
}

QRectF Projectile::boundingRect() const {
    return QGraphicsPixmapItem::boundingRect();
}

QPainterPath Projectile::shape() const {
    QPainterPath path;
    path.addEllipse(boundingRect());
    return path;
}
