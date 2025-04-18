#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QObject>
#include "projectile.h"
#include <QTimer>
#include <QVector>

enum Direction {
    Up,
    Down,
    Left,
    Right
};

class Player : public QObject, public QGraphicsPixmapItem {
Q_OBJECT

public:
    Player(QGraphicsItem *parent = nullptr);
    void shoot(int tileSize);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:

    void nextFrame();

private:
    QVector<QPixmap> walkFrames;
    int currentFrame;
    QTimer *animationTimer;
    int step;
    qreal rotationAngle;// 0 = bas, 90 = gauche, etc.
    bool moving = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keyLeft = false;
    bool keyRight = false;
    Direction direction = Down; // Par défaut vers le bas
    bool canShoot = true;
    QTimer* shootCooldownTimer;

    signals:
    void gameOver();
};

#endif // PLAYER_H
