#include "menu.h"
#include <QVBoxLayout>
#include <QPushButton>

Menu::Menu(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    setStyleSheet("background-color: black; color: white;");

    QStringList levels = { "niveau1", "niveau2", "niveau3" };
    for (int i = 0; i < levels.size(); ++i) {
        QPushButton* btn = new QPushButton("Niveau " + QString::number(i + 1), this);
        layout->addWidget(btn);
        connect(btn, &QPushButton::clicked, this, [=]() {
            emit levelSelected("../editeur de map/map/map-" + levels[i] + ".json");
        });
    }
}