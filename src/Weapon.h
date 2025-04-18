#ifndef WEAPON_H
#define WEAPON_H

#include <QObject>
#include <QString>

class Player;
class QGraphicsScene;

class Weapon : public QObject {
Q_OBJECT

public:
    explicit Weapon(QObject* parent = nullptr) : QObject(parent) {}
    virtual void shoot(Player* player, QGraphicsScene* scene, int tileSize) = 0;
    virtual QString getSpritePath() const = 0;
    virtual QString getName() const = 0 ;
    virtual ~Weapon() = default;
};

#endif // WEAPON_H
