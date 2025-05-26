#include "MyScene.h"
#include <QDebug>


MyScene::MyScene(QObject* parent) : QGraphicsScene(parent) {
    this->setSceneRect(0, 0, 400, 800);  // même taille que ta fenêtre


}

void MyScene::load() {
    player = nullptr;
    enemy = nullptr;
    gameTimer = nullptr;
    spawnTimer = nullptr;
    movementTimer = nullptr;
    golemSpawnTimer = nullptr;
    boss = nullptr;
    healthBarBack = nullptr;
    healthBarFront = nullptr;
    healthText = nullptr;
    weaponText = nullptr;
    view = nullptr;
    scoreText = nullptr;
    tileSize = 32;

    QMap<int, QPixmap> tilesetMap;

    // Chargement des tilesets (mêmes chemins que dans le constructeur)
    addTileset(tilesetMap, "../editeur de map/Texture/Tileset_Grass.png",     1,    32, 32, 64);
   /*
    addTileset(tilesetMap, "../editeur de map/Texture/torche.png",  3057, 32, 32, 288);
    addTileset(tilesetMap, "../editeur de map/Texture/water.png",      3041, 32, 32, 48);
    addTileset(tilesetMap, "../editeur de map/Texture/TX Plant.png",          2801, 32, 32, 256);
    */
    //addTileset(tilesetMap,"../editeur de map/Texture/TX Tileset Wall.png",609,32,32,256);

    addTilesetsFromJson(selectedMapPath, tilesetMap, 32, 32);

    // Charge la carte dynamique choisie
    loadMapFromJson(selectedMapPath, tilesetMap, 32, 32);

    // Le reste est identique à ce que tu avais dans ton constructeur
    player = new Player();
    addItem(player);
    player->setPos(9*tileSize, 9*tileSize);
    player->setFocus();

    initHud();

    static int frameCount = 0;
    ++frameCount;

    if (frameCount % 300 == 0) {
        Enemy* enemy = new Enemy(player, 32);
        addItem(enemy);
    }

    Golem* golem = new Golem(player, 128);
    connect(golem, &Golem::golemDefeated, this, [this]() {
        increaseScore(20);
    });

    addItem(golem);
    golem->setPos(15 * tileSize, 13 * tileSize);
    if (frameCount % 500 == 0) {
        Golem* golem = new Golem(player, 128);
        connect(golem, &Golem::golemDefeated, this, [this]() {
            increaseScore(20);
        });

        addItem(golem);
        int tileX = QRandomGenerator::global()->bounded(16);
        int tileY = QRandomGenerator::global()->bounded(16);
        golem->setPos(tileX * tileSize, tileY * tileSize);
    }





    QGraphicsPixmapItem* projectile = new QGraphicsPixmapItem();
    projectile->setData(0, "player_projectile");
    addItem(projectile);


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
    if (!player) return;

    spawnEnemies();
    updateHealthBar();
    playerView();

    if (weaponText) weaponText->setPlainText("Arme : " + player->getCurrentWeaponName());
    updateHud();

    if (player->getHp() <= 0) {
        showGameOverScreen();
    }
}

void MyScene::updateHud() {
        if (!view) return;

        QPointF topLeft = view->mapToScene(0, 0);

        if (healthBarBack) healthBarBack->setPos(topLeft + QPointF(10, 10));
        if (healthBarFront) healthBarFront->setPos(topLeft + QPointF(10, 10));
        if (healthText) {
            healthText->setPos(topLeft + QPointF(5, 5));
            healthText->setPlainText(QString("HP : %1").arg(player ? player->getHp() : 0));
        }
        if (weaponText) weaponText->setPos(topLeft + QPointF(10, 30));
        if (scoreText) {
            scoreText->setPos(topLeft + QPointF(10, 50));
            scoreText->setPlainText(QString("Score : %1").arg(score));
        }
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

void MyScene::addTilesetsFromJson(const QString& jsonPath, QMap<int, QPixmap>& tilesetMap, int tileWidth, int tileHeight) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ Impossible d’ouvrir le fichier JSON :" << jsonPath;
        return;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    file.close();

    if (parseError.error != QJsonParseError::NoError || !doc.isObject()) {
        qWarning() << "❌ Erreur de parsing JSON :" << parseError.errorString();
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray tilesets = root["tilesets"].toArray();

    for (const QJsonValue& val : tilesets) {
        QJsonObject obj = val.toObject();

        if (!obj.contains("image")) {
            qWarning() << "Tileset externe ignoré (fichier .tsx non résolu automatiquement):" << obj["source"].toString();
            continue;
        }

        QString imagePath = obj["image"].toString().replace("..", "../editeur de map");  // ajuster si nécessaire
        int firstGid = obj["firstgid"].toInt();
        int imageWidth = obj["imagewidth"].toInt();
        int imageHeight = obj["imageheight"].toInt();
        int tileCount = obj["tilecount"].toInt();

        QPixmap tileset(imagePath);
        if (tileset.isNull()) {
            qWarning() << "❌ Échec du chargement du tileset :" << imagePath;
            continue;
        }

        int columns = imageWidth / tileWidth;

        for (int i = 0; i < tileCount; ++i) {
            int x = (i % columns) * tileWidth;
            int y = (i / columns) * tileHeight;
            QPixmap tile = tileset.copy(x, y, tileWidth, tileHeight);
            tilesetMap[firstGid + i] = tile;
        }

        qDebug() << "✅ Tileset chargé automatiquement :" << imagePath << "GID à partir de" << firstGid;
    }
}


void MyScene::loadMapFromJson(const QString& jsonPath, const QMap<int, QPixmap>& tilesetMap, int tileWidth, int tileHeight) {
    QFile file(jsonPath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "❌ Erreur : impossible d’ouvrir le fichier JSON :" << jsonPath;
        return;
    }

    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    file.close();
    if (!doc.isObject()) {
        qWarning() << "❌ JSON mal formé.";
        return;
    }

    QJsonObject root = doc.object();
    QJsonArray layers = root["layers"].toArray();
    int mapWidth = root["width"].toInt();
    int mapHeight = root["height"].toInt();

    for (const QJsonValue& layerVal : layers) {
        QJsonObject layer = layerVal.toObject();
        QString layerClass = layer["class"].toString();
        QJsonArray data = layer["data"].toArray();

        for (int i = 0; i < data.size(); ++i) {
            quint32 rawGid = static_cast<quint32>(data[i].toDouble());
            if (rawGid == 0) continue;

            TileTransform t = decodeGid(rawGid);
            if (!tilesetMap.contains(t.gid)) {
                qWarning() << "⚠️ GID inconnu :" << t.gid;
                continue;
            }

            int x = (i % mapWidth) * tileWidth;
            int y = (i / mapWidth) * tileHeight;

            if (layerClass == "collision") {
                QGraphicsRectItem* collider = new QGraphicsRectItem(0, 0, tileWidth, tileHeight);
                collider->setPos(x, y);
                collider->setBrush(Qt::transparent);
                collider->setPen(Qt::NoPen);
                collider->setData(0, "wall");
                collider->setZValue(1);
                addItem(collider);
                continue;
            }

            QPixmap baseTile = tilesetMap[t.gid];
            QGraphicsPixmapItem* tile = new QGraphicsPixmapItem(baseTile);
            tile->setPos(x, y);
            tile->setZValue(0);

            // Transformation autour du centre
            QTransform transform;
            QPointF center(tileWidth / 2.0, tileHeight / 2.0);

            if (t.flipD) {
                transform.translate(center.x(), center.y());
                transform.rotate(90);  // ou 270 selon orientation diagonale
                transform.translate(-center.x(), -center.y());
            }

            if (t.flipH || t.flipV) {
                qreal sx = t.flipH ? -1 : 1;
                qreal sy = t.flipV ? -1 : 1;
                transform.translate(center.x(), center.y());
                transform.scale(sx, sy);
                transform.translate(-center.x(), -center.y());
            }

            tile->setTransform(transform);
            addItem(tile);
        }
    }

    setSceneRect(0, 0, mapWidth * tileWidth, mapHeight * tileHeight);
}


void MyScene::handleGameOver() {
    if (gameTimer) gameTimer->stop();
    if (movementTimer) movementTimer->stop();

    // Texte Game Over
    QGraphicsTextItem* gameOverText = new QGraphicsTextItem("GAME OVER");
    gameOverText->setDefaultTextColor(Qt::red);
    gameOverText->setFont(QFont("Arial", 40, QFont::Bold));
    gameOverText->setZValue(10); // devant tout

    // Centrer le texte dans la vue
    if (view) {
        QPointF center = view->mapToScene(view->viewport()->rect().center());
        QRectF textRect = gameOverText->boundingRect();
        gameOverText->setPos(center.x() - textRect.width() / 2, center.y() - textRect.height() / 2);
    } else {
        // Si pas de vue, positionner au centre de la scène
        QRectF sceneRect = this->sceneRect();
        QRectF textRect = gameOverText->boundingRect();
        gameOverText->setPos(sceneRect.width()/2 - textRect.width()/2, sceneRect.height()/2 - textRect.height()/2);
    }

    addItem(gameOverText);
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
    if (!player || !healthBarFront || !healthText) return;
    int hp = player->getHp();
    int maxHp = 100;
    int fullWidth = 100;

    int newWidth = qMax(0, hp * fullWidth / maxHp);
    healthBarFront->setRect(0, 0, newWidth, 10);
    healthText->setPlainText(QString("HP : %1 / %2").arg(hp).arg(maxHp));
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
    // Arrêter timers
    if (gameTimer) gameTimer->stop();
    if (movementTimer) movementTimer->stop();

    clear();  // Efface tous les items de la scène (attention aux objets non supprimés manuellement !)

    // Ensuite supprime les pointeurs restants
    delete player; player = nullptr;
    delete healthBarBack; healthBarBack = nullptr;
    delete healthBarFront; healthBarFront = nullptr;
    delete healthText; healthText = nullptr;
    delete weaponText; weaponText = nullptr;
    delete scoreText; scoreText = nullptr;
    delete replayProxy; replayProxy = nullptr;
    delete replayButton; replayButton = nullptr;
    
    score = 0;
    load();

    if (gameTimer) gameTimer->start(16);
    if (movementTimer) movementTimer->start(16);
}


void MyScene::increaseScore(int amount) {
    score += amount;
    if (scoreText) {
        scoreText->setPlainText(QString("Score : %1").arg(score));
    }
}

void MyScene::showGameOverScreen() {
    // Stopper timers
    if (gameTimer) gameTimer->stop();
    if (movementTimer) movementTimer->stop();

    // Afficher texte Game Over
    if (!gameOverText) {
        gameOverText = new QGraphicsTextItem("GAME OVER");
        gameOverText->setDefaultTextColor(Qt::red);
        gameOverText->setFont(QFont("Arial", 40, QFont::Bold));
        gameOverText->setZValue(10);
        addItem(gameOverText);
    }

    // Centrer texte
    if (view) {
        QPointF center = view->mapToScene(view->viewport()->rect().center());
        QRectF rect = gameOverText->boundingRect();
        gameOverText->setPos(center.x() - rect.width() / 2, center.y() - rect.height() / 2 - 50);
    }

    // Bouton Rejouer
    if (!replayButton) {
        replayButton = new QPushButton("Rejouer");
        replayButton->setFixedSize(120, 50);

        replayProxy = addWidget(replayButton);
        replayProxy->setZValue(11);

        // Centrer bouton sous le texte
        if (view) {
            QPointF center = view->mapToScene(view->viewport()->rect().center());
            replayProxy->setPos(center.x() - replayButton->width() / 2, center.y() + 10);
        }

        // Connexion du bouton pour reset
        connect(replayButton, &QPushButton::clicked, this, &MyScene::resetGame);
    }

    // Désactiver interactions joueur (optionnel)
    if (player) player->setEnabled(false);
}

void MyScene::initHud()
{
    int barWidth = 100;
    int barHeight = 12;
    int barX = 10;
    int barY = 10;

    // --- Barre de fond (gris foncé, avec bord blanc)
    healthBarBack = new QGraphicsRectItem(0, 0, barWidth, barHeight);
    healthBarBack->setBrush(QColor(50, 50, 50));
    healthBarBack->setPen(QPen(Qt::white));
    healthBarBack->setZValue(2);
    healthBarBack->setPos(barX, barY);
    addItem(healthBarBack);

    // --- Barre de vie (rouge, avant-plan)
    healthBarFront = new QGraphicsRectItem(0, 0, barWidth, barHeight);
    healthBarFront->setBrush(Qt::red);
    healthBarFront->setPen(Qt::NoPen);
    healthBarFront->setZValue(3);
    healthBarFront->setPos(barX, barY);
    addItem(healthBarFront);

    // --- Texte HP
    QString hpText = player ? QString("HP : %1 / %2")
            .arg(player->getHp())
            .arg(100)
                            : "HP : ?";
    healthText = new QGraphicsTextItem(hpText);
    healthText->setDefaultTextColor(Qt::white);
    healthText->setFont(QFont("Consolas", 10, QFont::Bold));
    healthText->setZValue(4);
    healthText->setPos(barX + barWidth + 5, barY - 2); // À droite de la barre
    addItem(healthText);

    // --- Texte Arme
    QString weaponName = player ? player->getCurrentWeaponName() : "Aucune";
    weaponText = new QGraphicsTextItem("Arme : " + weaponName);
    weaponText->setDefaultTextColor(Qt::white);
    weaponText->setFont(QFont("Consolas", 10, QFont::Bold));
    weaponText->setZValue(4);
    weaponText->setPos(barX, barY + barHeight + 10); // Sous la barre
    addItem(weaponText);

    // --- Texte Score
    scoreText = new QGraphicsTextItem("Score : 0");
    scoreText->setDefaultTextColor(Qt::yellow);
    scoreText->setFont(QFont("Consolas", 10, QFont::Bold));
    scoreText->setZValue(4);
    scoreText->setPos(barX, barY + barHeight + 30); // Sous l'arme
    addItem(scoreText);
}


