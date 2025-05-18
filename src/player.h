#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QObject>
#include "projectile.h"
#include <QTimer>
#include <QVector>
#include "Weapon.h"
#include "bat.h"
#include "Gun.h"

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

    Direction getDirection() const { return direction; }

    void shoot(int tileSize);

    int getHp() const { return hp; }
    void takeDamage(int amount);
    void setStep(float newStep) { step = newStep; }


    void startLavaDamage();
    void stopLavaDamage();

    void initWeaponInventory();
    void setWeapon(Weapon* weapon);
    void switchWeapon(int index);
    QString getCurrentWeaponName() const;

    const QMap<int,Weapon*>& getInventory() const { return weaponInventory; }
    void handleKeyPress(QKeyEvent* event);
    void handleKeyRelease(QKeyEvent* event);
    void updateMovement();
protected:


private slots:
    void nextFrame();


private:
    int hp =100;

    QVector<QPixmap> walkFrames;
    int currentFrame;
    QTimer* animationTimer;
    float step;
    qreal rotationAngle;// 0 = bas, 90 = gauche, etc.
    bool moving = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keyLeft = false;
    bool keyRight = false;
    Direction direction = Down; // Par défaut vers le bas
    bool canShoot = true;

    QTimer* shootCooldownTimer;
    QTimer* lavaDamageTimer = nullptr;
    QTimer* iceTimer = nullptr;
    QTimer* damageFlashTimer = nullptr;
    QTimer* movementTimer = nullptr;

    void flashRed();
    void loadWalkAnimation();
    void setupAnimationTimer();
    void setupShootCooldownTimer();
    void setupLavaDamageTimer();
    void setupDamageFlashTimer();

    QMap<int, Weapon*> weaponInventory;
    Weapon* currentWeapon = nullptr;





    signals:
        void gameOver();
};

#endif // PLAYER_H
