
#ifndef CPP_QT_TPMINIPROJET_MENU_H
#define CPP_QT_TPMINIPROJET_MENU_H

#pragma once
#include <QWidget>

class Menu : public QWidget {
Q_OBJECT
public:
    Menu(QWidget* parent = nullptr);

signals:
    void levelSelected(const QString& mapPath);
};



#endif //CPP_QT_TPMINIPROJET_MENU_H
