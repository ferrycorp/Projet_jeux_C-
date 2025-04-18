#ifndef CPP_QT_TPMINIPROJET_MYSCENE_H
#define CPP_QT_TPMINIPROJET_MYSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "Player.h"
#include "mob.h"
#include <QDebug>
#include <vector>
#include <cstdlib>  // pour rand()
#include <ctime>

class MyScene : public QGraphicsScene {
    Q_OBJECT

public:
    MyScene(QObject* parent = nullptr);
    virtual ~MyScene();
    Player* getPlayer() const { return player; }

private slots:
    void updateGame(); // appelée régulièrement
    void handleGameOver();

private:
    Player* player;
    Enemy* enemy;
    QTimer* gameTimer;
    QTimer* spawnTimer;
    int tileSize;

    void generateBiomeMap(int rows, int cols, std::vector<std::vector<int>>& map);
    void loadMap();
    void spawnEnemies();
    void updateHealthBar();
    void checkEnvironmentEffects();

    QGraphicsRectItem* healthBarBack = nullptr;
    QGraphicsRectItem* healthBarFront = nullptr;
    QGraphicsTextItem* healthText = nullptr;

    QGraphicsTextItem* weaponText = nullptr;



};

#endif //CPP_QT_TPMINIPROJET_MYSCENE_H
