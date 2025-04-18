#include "MyScene.h"
#include <QDebug>

MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {
    this->setSceneRect(0, 0, 400, 800);  // même taille que ta fenêtre

    loadMap();

    // Création du joueur
    player = new Player();
    addItem(player);
    int tileSize = 32;
    player->setPos(1 * tileSize, 1 * tileSize);; // Position initiale

    // Test mob
    for (int i = 0; i < 3; ++i) {
        Enemy* enemy = new Enemy(player, tileSize);
        addItem(enemy);
    }

    // Timer principal pour la boucle de jeu
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &MyScene::updateGame);
    gameTimer->start(16); // 60 FPS (~16ms)

    connect(player, &Player::gameOver, this, &MyScene::handleGameOver);
}

MyScene::~MyScene() {
    delete gameTimer;
    // Pas besoin de delete player, Qt le fait via QGraphicsScene
}

void MyScene::updateGame() {
    // Ici on pourra gérer les tirs, les collisions, etc.
    static int frameCount = 0;
    ++frameCount;

    // toutes les 300 frames (~5 secondes à 60 FPS)
    if (frameCount % 300 == 0) {
        Enemy* enemy = new Enemy(player, tileSize);
        addItem(enemy);
    }
}

void MyScene::loadMap() {
    // Exemple simple de carte 10x10
    int map[10][10] = {
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 0, 0, 1, 1, 0, 0, 0, 1},
            {0, 0, 0, 0, 1, 1, 0, 0, 0, 1},
            {1, 0, 1, 0, 0, 0, 0, 1, 0, 1},
            {1, 0, 1, 1, 0, 0, 1, 1, 0, 1},
            {1, 0, 0, 0, 0, 0, 0, 0, 0, 1},
            {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}
    };

    int tileSize = 32; // taille d'une tuile dans le jeu (pixel-art standard)

    QPixmap wallPixmap("../images/mur.jpg");
    QPixmap floorPixmap("../images/sol.jpg");

    // Redimensionner les images à 32x32 pour uniformiser l'affichage
    wallPixmap = wallPixmap.scaled(tileSize, tileSize);
    floorPixmap = floorPixmap.scaled(tileSize, tileSize);

    for (int row = 0; row < 10; ++row) {
        for (int col = 0; col < 10; ++col) {
            QPixmap pixmap = (map[row][col] == 1) ? wallPixmap : floorPixmap;

            QGraphicsPixmapItem* tile = new QGraphicsPixmapItem(pixmap);
            tile->setPos(col * tileSize, row * tileSize);
            if (map[row][col] == 1) {
                tile->setData(0, "wall");  // 🔥 ceci est un mur
            }
            this->addItem(tile);
        }
    }

    // Redimensionne la scène à la taille totale de la map
    this->setSceneRect(0, 0, 10 * tileSize, 10 * tileSize);
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