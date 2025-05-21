#include "Boss.h"
#include <QDebug>

Boss::Boss(Player* player, int tileSize, QGraphicsItem* parent)
        : QObject(), QGraphicsPixmapItem(parent), player(player)
{
    // 1) Charger les frames
    loadAnimationFrames();

    // 2) Afficher la première image
    if (!animationFrames.isEmpty()) {
        setPixmap(animationFrames[0]);
        setTransformationMode(Qt::SmoothTransformation);
    }
    setScale(static_cast<qreal>(tileSize) / animationFrames[0].width());

    // 3) Ajuster la taille de la hitbox éventuelle
    setOffset(-tileSize / 2, -tileSize / 2);   // facultatif : centre l’origine
     // car nos frames font 64×64

    // 4) Placer le boss (exemple : centre de la scène)
       // ajuste comme tu veux

    // 5) Préparer et démarrer le timer d’animation
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &Boss::updateAnimation);
    startAnimation();
}

void Boss::loadAnimationFrames()
{
    // Exemple : charge des images boss_1.png à boss_6.png
    for (int i = 1; i <= 6; ++i) {
        QPixmap frame(QString("../boss_demon_slime/boss_demon_slime/individual sprites/01_demon_idle/demon_idle_%1.png").arg(i));
        if (!frame.isNull()) {
            animationFrames.append(frame); // ajuste la taille selon ton besoin
        }
    }

    if (animationFrames.isEmpty()) {
        qDebug() << "Erreur : aucune frame d'animation chargée pour le Boss.";
    }
}

void Boss::startAnimation()
{
    if (!animationFrames.isEmpty() && !animationTimer->isActive()) {
        animationTimer->start(200); // tous les 200ms
    }
}

void Boss::updateAnimation()
{
    if (animationFrames.isEmpty()) return;

    currentFrame = (currentFrame + 1) % animationFrames.size();
    setPixmap(animationFrames[currentFrame]);
}

void Boss::takeDamage(int amount)
{
    hp -= amount;
    qDebug() << "Le boss prend" << amount << "dégâts. HP restant :" << hp;

    if (hp <= 0) {
        qDebug() << "Le boss est vaincu !";
        deleteLater();
    }
}
//
// Created by mgicq on 20/05/2025.
//

#include "Boss.h"
