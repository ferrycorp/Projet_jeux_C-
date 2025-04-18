#ifndef GUN_H
#define GUN_H

#include "weapon.h"
#include <QGraphicsScene>

class Gun : public Weapon {
public:
    void shoot(Player* player, QGraphicsScene* scene, int tileSize) override;
    QString getSpritePath() const override;
    QString getName() const;
};

#endif // GUN_H
