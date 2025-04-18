#include "bat.h"
#include <QDebug>

void Bat::shoot(Player* player, QGraphicsScene* scene, int tileSize) {
    qDebug() << "Batte utilisée ! (pas encore d'effet de mêlée)";
    // plus tard : vérifier les ennemis proches et les frapper
}

QString Bat::getSpritePath() const {
    return "../images/batte.png";
}

QString Bat::getName() const {
    return "Batte de baseball";
}
