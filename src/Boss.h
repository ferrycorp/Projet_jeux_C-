#ifndef BOSS_H
#define BOSS_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QVector>
#include <QPixmap>
#include "player.h"

class Player;           // forward-decl – on ne touche pas au header Player ici

class Boss : public QObject, public QGraphicsPixmapItem
{
Q_OBJECT
public:
    /// Spawne un boss à tileSize × tileSize et démarre l'animation idle.
    explicit Boss(Player* player,
                  int      tileSize,
                  QGraphicsItem* parent = nullptr);

    void takeDamage(int amount);

private slots:
    void updateAnimation();

private:
    void loadAnimationFrames();
    void startAnimation();

    Player* player = nullptr;          // si tu veux l’utiliser plus tard
    QVector<QPixmap> animationFrames;  // frames idle
    int currentFrame = 0;

    QTimer* animationTimer = nullptr;

    int hp = 300;                      // points de vie du boss
};

#endif // BOSS_H
