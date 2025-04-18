#ifndef BAT_H
#define BAT_H

#include "weapon.h"

class Bat : public Weapon {
public:
    void shoot(Player* player, QGraphicsScene* scene, int tileSize) override;
    QString getSpritePath() const override;
    QString getName() const;
};

#endif // BAT_H
