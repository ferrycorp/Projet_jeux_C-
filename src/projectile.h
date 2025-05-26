#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QGraphicsPixmapItem>
#include <QObject>
#include <QTimer>
#include <QPointF>
#include <QPainterPath>
#include "Golem.h"

// Forward declarations
class Enemy;
class Golem;

class Projectile : public QObject, public QGraphicsPixmapItem
{
Q_OBJECT

public:
    explicit Projectile(qreal angleDeg, int tileSize, QGraphicsItem* parent = nullptr);
    ~Projectile();

    // Redéfinition des méthodes virtuelles
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

private slots:
    void moveForward();
    void destroySelf();

private:
    void checkCollisions();

    QTimer* moveTimer;
    QTimer* lifetimeTimer;
    QPointF direction;
    qreal speed;
};

#endif // PROJECTILE_H