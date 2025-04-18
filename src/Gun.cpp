#include "gun.h"
#include "projectile.h"
#include "player.h"

void Gun::shoot(Player* player, QGraphicsScene* scene, int tileSize) {
    qreal angle = 0;

    switch (player->getDirection()) {
        case Up:    angle = 180; break;
        case Down:  angle = 0;   break;
        case Left:  angle = -90; break;
        case Right: angle = 90;  break;
    }

    Projectile* proj = new Projectile(angle, tileSize);
    proj->setPos(player->pos().x() + tileSize / 2, player->pos().y() + tileSize / 2);

    scene->addItem(proj);
}

QString Gun::getSpritePath() const {
    return "../images/pistolet.png";
}

QString Gun::getName() const {
    return "Pistolet";
}
