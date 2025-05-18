#include "MainWindow.h"
#include "Player.h"
#include <QKeyEvent>
#include <QMessageBox>
#include "MyScene.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    // Création de la scène
    levelMenu = new Menu(this);
    setCentralWidget(levelMenu);
    resize(400, 800);
    setWindowTitle("Sélection du niveau");

    connect(levelMenu, &Menu::levelSelected, this, &MainWindow::startGameWithLevel);
    // Menu d'aide
    helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction* actionHelp = new QAction(tr("&About"), this);
    connect(actionHelp, &QAction::triggered, this, &MainWindow::slot_aboutMenu);
    helpMenu->addAction(actionHelp);
}

MainWindow::~MainWindow() {
    // Qt gère la mémoire, pas besoin de delete
}

void MainWindow::slot_aboutMenu() {
    QMessageBox msgBox;
    msgBox.setText("A small QT/C++ project...");
    msgBox.setModal(true);
    msgBox.exec();
}

void MainWindow::startGameWithLevel(const QString& mapPath) {
    mainScene = new MyScene;
    mainScene->setMapPath(mapPath);
    mainScene->load(); // Charge la map

    mainView = new QGraphicsView;
    mainView->setScene(mainScene);
    mainScene->setView(mainView);
    mainView->scale(4, 4);

    setCentralWidget(mainView);  // <-- D'abord ça
    mainView->setFocusPolicy(Qt::StrongFocus);  // Ensuite ces lignes
    mainView->setFocus();


    Player* player = mainScene->getPlayer();
    if (player) {
        player->setFlag(QGraphicsItem::ItemIsFocusable);
        player->setFocus();  // Et enfin ça !
    }

    inventory = new Inventory(player->getInventory(), this);
    connect(inventory, &Inventory::weaponSelected, player, &Player::switchWeapon);

    QShortcut* inventoryShortcut = new QShortcut(QKeySequence(Qt::Key_I), this);
    connect(inventoryShortcut, &QShortcut::activated, this, [this]() {
        inventory->show();
    });
}

