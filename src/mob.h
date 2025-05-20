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
    void takeDamage(int amount);

    private slots:
        void moveTowardPlayer();
        void checkCollisionWithPlayer();

private:
    int health = 2;
    QTimer* moveTimer;
    Player* targetPlayer;
    int speed;
    QVector<QPixmap> walkFrames;
    int currentFrame = 0;
    QTimer* animationTimer = nullptr;
};

#endif // MOB_H