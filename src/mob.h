#ifndef MOB_H
#define MOB_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include <QVector>
#include "player.h"

class Enemy : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    Enemy(Player* player, int tileSize, QGraphicsItem* parent = nullptr);

    private slots:
        void moveTowardPlayer();
        void checkCollisionWithPlayer();

private:
    QTimer* moveTimer;
    Player* targetPlayer;
    int speed;
};

#endif // MOB_H