#ifndef CPP_QT_TPMINIPROJET_MYSCENE_H
#define CPP_QT_TPMINIPROJET_MYSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "Player.h"
#include "mob.h"
#include <QDebug>

class MyScene : public QGraphicsScene {
    Q_OBJECT

public:
    MyScene(QObject* parent = nullptr);
    virtual ~MyScene();

private slots:
    void updateGame(); // appelée régulièrement

private:
    void loadMap();
    Player* player;
    Enemy* enemy;
    QTimer* gameTimer;
    QTimer* spawnTimer;
    int tileSize;

};

#endif //CPP_QT_TPMINIPROJET_MYSCENE_H
