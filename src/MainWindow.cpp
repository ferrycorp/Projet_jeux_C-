#include "MainWindow.h"
#include "Player.h"
#include <QKeyEvent>
#include <QMessageBox>
#include "MyScene.h"

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent)
{
    // Création de la scène
    mainScene = new MyScene;

    // Création de la vue
    mainView = new QGraphicsView;
    mainView->setScene(mainScene);
    mainScene->setView(mainView);
    mainView->setFocus();
    mainView->scale(4, 4);


    QShortcut* inventoryShortcut = new QShortcut(QKeySequence(Qt::Key_I), this);
    connect(inventoryShortcut, &QShortcut::activated, this, [this]() {
        inventory->show();
    });

    // Fenêtre principale
    setCentralWidget(mainView);
    setWindowTitle("My main window");
    resize(400, 800);

    // Menu d'aide
    helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction* actionHelp = new QAction(tr("&About"), this);
    connect(actionHelp, &QAction::triggered, this, &MainWindow::slot_aboutMenu);
    helpMenu->addAction(actionHelp);

    // 🔄 Création de la fenêtre d'inventaire
    Player* player = mainScene->getPlayer();
    inventory = new Inventory(player->getInventory(), this); // ← renommé ici
    connect(inventory, &Inventory::weaponSelected, player, &Player::switchWeapon);
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
