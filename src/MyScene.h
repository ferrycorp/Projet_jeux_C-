#ifndef CPP_QT_TPMINIPROJET_MYSCENE_H
#define CPP_QT_TPMINIPROJET_MYSCENE_H

#include <QGraphicsScene>
#include <QGraphicsView>
#include <QTimer>
#include <QPushButton>
#include "Player.h"
#include "mob.h"
#include "Boss.h"
#include "Golem.h"
#include "projectile.h"
#include <QDebug>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QXmlStreamReader>
#include <cstdlib>   // for rand()
#include <ctime>
#include <QApplication>
#include <QGraphicsProxyWidget>
#include <QRandomGenerator>


struct TileAnimation {
    QList<QPixmap> frames;
    QList<int> durations;
    int currentIndex = 0;
    int elapsed = 0;
};

struct TileTransform {
    int gid;
    bool flipH = false;
    bool flipV = false;
    bool flipD = false;
};

static TileTransform decodeGid(quint32 rawGid) {
    TileTransform result;
    result.flipH = rawGid & 0x80000000;
    result.flipV = rawGid & 0x40000000;
    result.flipD = rawGid & 0x20000000;
    result.gid   = rawGid & ~(0x80000000 | 0x40000000 | 0x20000000);
    return result;
}


class MyScene : public QGraphicsScene {
    Q_OBJECT

public:
    MyScene(QObject* parent = nullptr);
    virtual ~MyScene();
    Player* getPlayer() const { return player; }
    void setView(QGraphicsView* v) { view = v; }
    void load();
    void setMapPath(const QString& path) {
        selectedMapPath = path;
    }
    QString getMapPath() const { return selectedMapPath; }
    void increaseScore(int amount);

protected:
    void keyPressEvent(QKeyEvent* event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:
    void updateGame(); // appelée régulièrement
    void handleGameOver();
    void resetGame();
    void updateMovement();
    void updateHud();



private:
    QGraphicsTextItem* gameOverText = nullptr;
    QPushButton* replayButton = nullptr;
    QGraphicsProxyWidget* replayProxy = nullptr;
    Player* player;
    Enemy* enemy;
    QTimer* gameTimer;
    QTimer* spawnTimer;
    int tileSize;
    QString selectedMapPath;

    QTimer* movementTimer;
    QTimer* golemSpawnTimer;

    QList<QPointer<Golem>> golems; // Au lieu de QList<Golem*>


        void loadMapFromJson(const QString& jsonPath, const QMap<int, QPixmap>& tilesetMap, int tileWidth, int tileHeight);
    void addTileset(QMap<int, QPixmap>& tilesetMap,const QString& imagePath,int firstGid,int tileWidth,int tileHeight,int tileCount);

    void addTilesetsFromJson(const QString& jsonPath, QMap<int, QPixmap>& tilesetMap, int tileWidth, int tileHeight);



        QMap<int, TileAnimation> animatedTileData;
    QMap<int, QList<QGraphicsPixmapItem*>> animatedTileInstances;

    Boss* boss = nullptr;
    void spawnEnemies();
    void updateHealthBar();
    void playerView();

    QGraphicsRectItem* healthBarBack = nullptr;
    QGraphicsRectItem* healthBarFront = nullptr;
    QGraphicsTextItem* healthText = nullptr;

    QGraphicsTextItem* weaponText = nullptr;
    QGraphicsView* view = nullptr;

    QGraphicsTextItem* scoreText = nullptr;
    int score = 0;  // Valeur initiale du score

    void showGameOverScreen();
    void initHud();



signals:
    void gameOver();


};

#endif //CPP_QT_TPMINIPROJET_MYSCENE_H
