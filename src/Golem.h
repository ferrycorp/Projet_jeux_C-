#ifndef GOLEM_H
#define GOLEM_H

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QPixmap>
#include <QVector>
#include <QPointF>
#include <QGraphicsColorizeEffect>
#include <QGraphicsScene>
#include <QDebug>
#include <QPainterPath>
#include <cmath>
#include <cstdlib>
#include "player.h"
#include <QObject>
#include <QPainter>
#include <QGraphicsView>



// Forward declarations
class Player;
class Projectile;

enum GolemState {
    IDLE,
    WALKING,
    FIRING,
    CHARGING,
    HIT,
    DYING
};

class Golem : public QObject, public QGraphicsPixmapItem
{
Q_OBJECT

public:
    // Constantes de configuration
    static constexpr int MAX_HEALTH = 100;
    static constexpr int BASE_SPEED = 2;
    static constexpr int CHARGE_SPEED = 4;
    static constexpr int ANIM_FRAME_TIME = 100;
    static constexpr int ACTION_INTERVAL = 100;
    static constexpr int PROJECTILE_COOLDOWN = 2000;
    static constexpr int DETECTION_RANGE = 300;
    static constexpr int ATTACK_RANGE = 150;

    bool isDying = false;

    explicit Golem(Player* targetPlayer, int spriteSize = 64, QGraphicsItem* parent = nullptr);
    ~Golem();

    // Méthodes de collision améliorées
    QRectF boundingRect() const override;
    QPainterPath shape() const override;

    // Méthodes publiques
    void takeDamage(int amount);
    void update();
    int getHealth() const { return health; }
    GolemState getCurrentState() const { return currentState; }

    // Méthodes de collision publiques
    QRectF getHitbox() const;
    bool isPointInHitbox(const QPointF& point) const;

signals:
    void golemDefeated();

private slots:
    void updateAnimation();
    void performAction();
    void fireProjectile();
    void flashDamage();
    void checkCollisions();

private:
    // Attributs du golem
    Player* player;
    int health;
    int spriteSize;
    GolemState currentState;
    double speed;
    int shotCount = 0;

    // Animation
    int currentFrame;
    int animationRow;
    QVector<QPixmap> animationFrames;
    QVector<QPixmap> idleFrames;
    QVector<QVector<QPixmap>> animations;
    QVector<QPixmap> deathFrames;


    // États et timers
    bool isFlashing;
    bool canTakeDamage;
    int attackCooldown;
    int attackCounter;

    QTimer* animationTimer;
    QTimer* actionTimer;
    QTimer* projectileTimer;
    QTimer* chargeTimer;
    QTimer* damageTimer;
    QTimer* collisionTimer;


    // Collision
    QRectF hitboxRect;  // Hitbox relative au sprite

    // Méthodes privées
    void loadAnimations();
    QVector<QPixmap> loadAnimationFrames(const QString& folder, const QString& baseName, int frameCount);
    void changeState(GolemState newState);
    void moveTowardsPlayer(double moveSpeed);
    QPointF calculateMovementVector(double moveSpeed);
    void dropLoot();
    void setupCollisionBox();


    // Méthodes de collision privées
    void handleProjectileCollision(QGraphicsItem* projectile);
};

#endif // GOLEM_H