#ifndef CPP_QT_TPMINIPROJET_MYSCENE_H
#define CPP_QT_TPMINIPROJET_MYSCENE_H

#include <QGraphicsScene>
#include <QTimer>
#include "Player.h"

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
    QTimer* gameTimer;

};

#endif //CPP_QT_TPMINIPROJET_MYSCENE_H
