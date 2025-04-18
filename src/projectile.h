#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>

class Projectile : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    Projectile(qreal angleDeg, int tileSize, QGraphicsItem* parent = nullptr);

    private slots:
        void moveForward();

private:
    QTimer* moveTimer;
    QPointF direction;
    int speed;
};

#endif // PROJECTILE_H