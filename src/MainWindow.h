#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QMenu>
#include "MyScene.h"
#include "Inventory.h"
#include <QMenuBar>
#include <QAction>
#include <QMessageBox>
#include "player.h"
#include <QKeyEvent>
#include <QShortcut>
#include <QKeySequence>
#include "menu.h"

class MainWindow : public QMainWindow {
Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:


private slots:
    void slot_aboutMenu();
    void startGameWithLevel(const QString& mapPath);

private:
    QGraphicsView* mainView;
    MyScene* mainScene;
    QMenu* helpMenu;
    Inventory* inventory = nullptr; // ← renommé ici
    Menu* levelMenu = nullptr;

    };

#endif // MAINWINDOW_H
