#ifndef INVENTORYDIALOG_H
#define INVENTORYDIALOG_H

#include <QDialog>
#include <QMap>
#include "weapon.h"
#include "Gun.h"
#include "bat.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QIcon>

class QPushButton;

class Inventory : public QDialog {
Q_OBJECT

public:
    explicit Inventory(const QMap<int, Weapon*>& inventory, QWidget* parent = nullptr);

signals:
    void weaponSelected(int slot);

private:
    QMap<int, QPushButton*> buttons;
};
#endif // INVENTORYDIALOG_H
