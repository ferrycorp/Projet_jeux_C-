#include "MyScene.h"
#include <QDebug>


MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {
    this->setSceneRect(0, 0, 400, 800);  // même taille que ta fenêtre


}

void MyScene::load() {
    tileSize = 32;

    QMap<int, QPixmap> tilesetMap;

    // Chargement des tilesets (mêmes chemins que dans le constructeur)
    addTileset(tilesetMap, "../editeur de map/Texture/Tileset_Grass.png",     1,    32, 32, 64);
    addTileset(tilesetMap, "../editeur de map/Texture/pillar.png",    65,   32, 32, 2688);
    addTileset(tilesetMap, "../editeur de map/Texture/torche.png",  3057, 32, 32, 288);
    addTileset(tilesetMap, "../editeur de map/Texture/water.png",      3041, 32, 32, 48);
    addTileset(tilesetMap, "../editeur de map/Texture/TX Plant.png",          2801, 32, 32, 256);

    // Charge la carte dynamique choisie
    loadMapFromJson(selectedMapPath, tilesetMap, 32, 32);

    // Le reste est identique à ce que tu avais dans ton constructeur
    player = new Player();
    addItem(player);
    player->setPos(32, 32);
    player->setFocus();


    int barWidth = 100;
    int barHeight = 10;

    healthBarBack = new QGraphicsRectItem(0, 0, barWidth, barHeight);
    healthBarBack->setBrush(Qt::gray);
    healthBarBack->setZValue(2);
    addItem(healthBarBack);

    healthBarFront = new QGraphicsRectItem(0, 0, barWidth, barHeight);
    healthBarFront->setBrush(Qt::red);
    healthBarFront->setZValue(3);
    addItem(healthBarFront);

    healthBarBack->setPos(10, 10);
    healthBarFront->setPos(10, 10);

    healthText = new QGraphicsTextItem("HP : 100 / 100");
    healthText->setDefaultTextColor(Qt::white);
    healthText->setFont(QFont("Arial", 10));
    healthText->setZValue(4);
    healthText->setPos(10, 5);
    addItem(healthText);

    weaponText = new QGraphicsTextItem("Arme : " + player->getCurrentWeaponName());
    weaponText->setDefaultTextColor(Qt::white);
    weaponText->setFont(QFont("Arial", 10));
    weaponText->setPos(10, 40);
    weaponText->setZValue(5);
    addItem(weaponText);

    for (int i = 0; i < 3; ++i) {
        Enemy* enemy = new Enemy(player, tileSize);
        addItem(enemy);
    }

    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MyScene::updateGame);
    gameTimer->start(16);
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &MyScene::updateMovement);
    movementTimer->start(16); // ~60 FPS

    //connect(player, &Player::gameOver, this, &MyScene::handleGameOver);
}


    MyScene::~MyScene() {
    delete gameTimer;
    // Pas besoin de delete player, Qt le fait via QGraphicsScene
}

void MyScene::updateGame() {
    // Ici on pourra gérer les tirs, les collisions, etc.
    spawnEnemies();
    updateHealthBar();
    checkEnvironmentEffects();
    playerView();
    weaponText->setPlainText("Arme : " + player->getCurrentWeaponName());
}


void MyScene::addTileset(QMap<int, QPixmap>& tilesetMap,
                         const QString& imagePath,
                         int firstGid,
                         int tileWidth,
                         int tileHeight,
                         int tileCount) {
    QPixmap tileset(imagePath);
    if (tileset.isNull()) {
        qWarning() << "⚠️ Impossible de charger le tileset :" << imagePath;
        return;
    }

    int columns = tileset.width() / tileWidth;
    for (int i = 0; i < tileCount; ++i) {
        int x = (i % columns) * tileWidth;
        int y = (i / columns) * tileHeight;
        tilesetMap[firstGid + i] = tileset.copy(x, y, tileWidth, tileHeight);
    }

    qDebug() << "✅ Tileset chargé :" << imagePath << "à partir de GID" << firstGid;
}


/*void MyScene::generateBiomeMap(int rows, int cols, std::vector<std::vector<int>>& map) {
    map.resize(rows, std::vector<int>(cols, 0));
    srand(static_cast<unsigned>(time(nullptr)));

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            // Bords extérieurs en murs
            if (row == 0 || col == 0 || row == rows - 1 || col == cols - 1) {
                map[row][col] = 1;
                continue;
            }

            // Génération aléatoire des biomes
            int r = rand() % 100;

            if (r < 60) map[row][col] = 0;         // sol normal
            else if (r < 75) map[row][col] = 2;    // lave
            else if (r < 85) map[row][col] = 3;    // glace
            else if (r < 95) map[row][col] = 4;    // herbe
            else map[row][col] = 1;                // mur
        }
    }
}

void MyScene::loadMap() {
    const int tileSize = 32;

    // Carte avec plusieurs environnements :
    // 0 = sol normal, 1 = mur, 2 = lave, 3 = glace, 4 = herbe
    int rows = 15;
    int cols = 15;
    std::vector<std::vector<int>> map;
    generateBiomeMap(rows, cols, map);

    // Chargement des images
    QPixmap wallPixmap("../images/mur.jpg");
    QPixmap floorPixmap("../images/sol.jpg");
    QPixmap lavaPixmap("../images/lave.jpg");
    QPixmap icePixmap("../images/ice.jpg");
    QPixmap grassPixmap("../images/herbe.jpg");

    // Redimensionnement
    wallPixmap = wallPixmap.scaled(tileSize, tileSize);
    floorPixmap = floorPixmap.scaled(tileSize, tileSize);
    lavaPixmap = lavaPixmap.scaled(tileSize, tileSize);
    icePixmap = icePixmap.scaled(tileSize, tileSize);
    grassPixmap = grassPixmap.scaled(tileSize, tileSize);

    for (int row = 0; row < rows; ++row) {
        for (int col = 0; col < cols; ++col) {
            int value = map[row][col];
            QPixmap pixmap;

            // Sélection de la bonne texture
            switch (value) {
                case 0: pixmap = floorPixmap; break;
                case 1: pixmap = wallPixmap; break;
                case 2: pixmap = lavaPixmap; break;
                case 3: pixmap = icePixmap; break;
                case 4: pixmap = grassPixmap; break;
                default: pixmap = floorPixmap; break;
            }

            // Création de la tuile
            QGraphicsPixmapItem* tile = new QGraphicsPixmapItem(pixmap);
            tile->setPos(col * tileSize, row * tileSize);

            // Pour permettre la détection (collision, effet...)
            switch (value) {
                case 1: tile->setData(0, "wall"); break;
                case 2: tile->setData(0, "lava"); break;
                case 3: tile->setData(0, "ice"); break;
                case 4: tile->setData(0, "grass"); break;
                default: tile->setData(0, "floor"); break;
            }

            this->addItem(tile);
        }
    }

    // Adapter la taille de la scène à la nouvelle map
    this->setSceneRect(0, 0, cols * tileSize, rows * tileSize);
}*/

void MyScene::loadMapFromJson(const QString& jsonPath, const QMap<int, QPixmap>& tilesetMap, int tileWidth, int tileHeight) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ Erreur : impossible d’ouvrir le fichier JSON :" << jsonPath;
        return;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        qWarning() << "❌ Erreur : JSON mal formé.";
        return;
    }

    QJsonObject root = doc.object();
    int mapWidth = root["width"].toInt();
    int mapHeight = root["height"].toInt();
    QJsonArray layers = root["layers"].toArray();

    for (const QJsonValue& layerVal : layers) {
        QJsonObject layer = layerVal.toObject();

        if (layer["type"].toString() != "tilelayer")
            continue;

        QJsonArray data = layer["data"].toArray();
        for (int i = 0; i < data.size(); ++i) {
            int gid = data[i].toInt();
            if (gid == 0) continue; // 0 = aucune tuile placée

            int x = (i % mapWidth) * tileWidth;
            int y = (i / mapWidth) * tileHeight;

            if (tilesetMap.contains(gid)) {
                QGraphicsPixmapItem* tile = new QGraphicsPixmapItem(tilesetMap[gid]);
                tile->setPos(x, y);
                addItem(tile);
            }

        }
    }

    // Redimensionne automatiquement la scène à la map
    this->setSceneRect(0, 0, mapWidth * tileWidth, mapHeight * tileHeight);
}



void MyScene::handleGameOver() {

    // Afficher le message de fin
    QGraphicsTextItem* gameOverText = addText("Fin du jeu !");
    gameOverText->setDefaultTextColor(Qt::red);
    gameOverText->setFont(QFont("Arial", 24, QFont::Bold));
    gameOverText->setPos(width() / 2 - 100, height() / 2 - 50);
}

void MyScene::spawnEnemies() {
    static int frameCount = 0;
    ++frameCount;

    if (frameCount % 300 == 0) {
        Enemy* enemy = new Enemy(player, 32);
        addItem(enemy);
    }
}

void MyScene::updateHealthBar() {
    int hp = player->getHp();
    int maxHp = 100;
    int fullWidth = 100;

    int newWidth = qMax(0, hp * fullWidth / maxHp);
    healthBarFront->setRect(0, 0, newWidth, 10);
    healthText->setPlainText(QString("HP : %1 / %2").arg(hp).arg(maxHp));
}

void MyScene::checkEnvironmentEffects() {
    bool onLava = false;
    bool onIce = false;

    for (QGraphicsItem* item : items()) {
        if (player->collidesWithItem(item)) {
            QString type = item->data(0).toString();

            if (type == "lava") onLava = true;
            if (type == "ice")  onIce = true;
        }
    }

    if (onLava)
        player->startLavaDamage();
    else
        player->stopLavaDamage();

    player->setStep(onIce ? 1 : 3);
}

void MyScene::playerView() {
    if (view && player) {
        view->centerOn(player);  // 👀 centre la vue sur le joueur
    }
}

void MyScene::keyPressEvent(QKeyEvent* event) {
    if (player) {
        player->handleKeyPress(event);
    }
}

void MyScene::keyReleaseEvent(QKeyEvent* event) {
    if (player) {
        player->handleKeyRelease(event);
    }
}

void MyScene::updateMovement() {
    if (player) {
        player->updateMovement();
    }
}


void MyScene::resetGame() {
    clear();

    if (replayButton) {
        replayButton->hide();
        replayButton->deleteLater();
        replayButton = nullptr;
    }
    if (quitButton) {
        quitButton->hide();
        quitButton->deleteLater();
        quitButton = nullptr;
    }

    player = new Player();
    addItem(player);
    player->setPos(1 * tileSize, 1 * tileSize);
    connect(player, &Player::gameOver, this, &MyScene::handleGameOver);

    gameTimer->start();
    spawnTimer->start();

    for (int i = 0; i < 3; ++i) {
        spawnEnemies();
    }
}




