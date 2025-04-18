#include "MyScene.h"
#include <QDebug>


MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {
    this->setSceneRect(0, 0, 400, 800);  // même taille que ta fenêtre
    loadMap();

    // Création du joueur
    player = new Player();
    addItem(player);
    int tileSize = 32;
    player->setPos(32, 32); // Position initiale
    // Barre de vie : fond (gris) + remplissage (rouge)
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

// Position en haut à gauche
    healthBarBack->setPos(10, 10);
    healthBarFront->setPos(10, 10);

    healthText = new QGraphicsTextItem("HP : 100 / 100");
    healthText->setDefaultTextColor(Qt::white);
    healthText->setFont(QFont("Arial", 10));
    healthText->setZValue(4);
    healthText->setPos(10, 5); // juste sous la barre

    addItem(healthText);



    /* Test mob
    for (int i = 0; i < 3; ++i) {
        Enemy* enemy = new Enemy(player, tileSize);
        addItem(enemy);
    }*/

    // Timer principal pour la boucle de jeu
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MyScene::updateGame);
    gameTimer->start(16); // 60 FPS (~16ms)

    //connect(player, &Player::gameOver, this, &MyScene::handleGameOver);


    weaponText = new QGraphicsTextItem("Arme : " + player->getCurrentWeaponName());
    weaponText->setDefaultTextColor(Qt::white);
    weaponText->setFont(QFont("Arial", 10));
    weaponText->setPos(10, 40);
    weaponText->setZValue(5);
    addItem(weaponText);

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

    weaponText->setPlainText("Arme : " + player->getCurrentWeaponName());

}



void MyScene::generateBiomeMap(int rows, int cols, std::vector<std::vector<int>>& map) {
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
}


void MyScene::handleGameOver() {
    // Arrêter le jeu
    gameTimer->stop();
    spawnTimer->stop();

    // Supprimer tous les ennemis et projectiles
    for (QGraphicsItem* item : items()) {
        if (dynamic_cast<Enemy*>(item) || dynamic_cast<Projectile*>(item)) {
            removeItem(item);
            delete item;
        }
    }

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
        Enemy* enemy = new Enemy(player, tileSize);
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


