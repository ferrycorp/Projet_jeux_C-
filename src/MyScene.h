#ifndef CPP_QT_TPMINIPROJET_MYSCENE_H
#define CPP_QT_TPMINIPROJET_MYSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QPushButton>
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
    void setView(QGraphicsView* v) { view = v; }
private slots:
    void updateGame(); // appelée régulièrement
    void handleGameOver();
    void resetGame();

private:
    QPushButton* replayButton = nullptr;
    QPushButton* quitButton = nullptr;
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
    void playerView();

    QGraphicsRectItem* healthBarBack = nullptr;
    QGraphicsRectItem* healthBarFront = nullptr;
    QGraphicsTextItem* healthText = nullptr;

    QGraphicsTextItem* weaponText = nullptr;
    QGraphicsView* view = nullptr;






};

#endif //CPP_QT_TPMINIPROJET_MYSCENE_H
