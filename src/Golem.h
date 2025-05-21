#ifndef GOLEM_H
#define GOLEM_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include "Player.h"
#include "projectile.h"
#include <vector>
#include "mob.h"
#include "mobmort.h"


class Projectile; // Déclaration anticipée

class Golem : public QObject, public QGraphicsPixmapItem {
Q_OBJECT

public:
    Golem(Player* targetPlayer, int spriteSize, QGraphicsItem* parent = nullptr);
    ~Golem();

    const int ANIM_FRAME_TIME = 100; // Temps entre chaque frame d'animation en millisecondes
    const int ACTION_INTERVAL = 1000; // Intervalle entre chaque action en millisecondes
    const int PROJECTILE_COOLDOWN = 3000; // Temps de recharge pour les projectiles en millisecondes
    const int CHARGE_COOLDOWN = 5000; // Temps de recharge pour l'attaque de charge en millisecondes
    const int DETECTION_RANGE = 500; // Portée de détection du joueur
    const int ATTACK_RANGE = 200; // Portée d'attaque
    const int CHARGE_SPEED = 15; // Vitesse de charge
    const int CHARGE_DAMAGE = 20; // Dégâts infligés lors d'une charge
    const int BASE_SPEED = 2; // Vitesse de base
    const int MAX_HEALTH = 5; // Santé maximale

    enum GolemState {
        IDLE,
        WALKING,
        FIRING,
        CHARGING,
        HIT,
        DYING
    };

    void updateAnimation();
    void changeState(GolemState newState);
    void performAction();
    void fireProjectile();
    void startChargeAttack();
    void performChargeAttack();
    void endChargeAttack();
    void moveTowardsPlayer(double moveSpeed);
    QPointF calculateMovementVector(double moveSpeed);
    void rotateTowardsPlayer();
    void takeDamage(int amount);
    void flashDamage();
    void checkCollisions();
    void update();
    void dropLoot();

signals:
    void golemDefeated();

private:
    Player* player;
    int health;
    int spriteSize;
    GolemState currentState;
    double speed;
    int currentFrame;
    int animationRow;
    bool isFlashing;
    bool canTakeDamage;
    int attackCooldown;
    int attackCounter;
    QPointF chargeTarget;
    double chargeDirection;

    QTimer* animationTimer;
    QTimer* actionTimer;
    QTimer* projectileTimer;
    QTimer* chargeTimer;
    QTimer* damageTimer;
    QTimer* collisionTimer;

    QVector<QPixmap> animationFrames;

    QList<QList<QPixmap>> animations;
    QVector<QPixmap> idleFrames;
    QVector<QPixmap> walkFrames;
    QVector<QPixmap> attackFrames;

    void loadAnimations();
    QVector<QPixmap> loadAnimationFrames(const QString& folder, const QString& baseName, int frameCount);

    };

#endif // GOLEM_H
