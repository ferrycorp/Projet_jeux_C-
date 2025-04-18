#include "Inventory.h"
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QIcon>

Inventory::Inventory(const QMap<int, Weapon*>& inventory, QWidget* parent)
        : QDialog(parent)
{
    setWindowTitle("Inventaire");
    setModal(true);

    QGridLayout* grid = new QGridLayout;

    int row = 0, col = 0;

    for (auto it = inventory.constBegin(); it != inventory.constEnd(); ++it) {
        int slot = it.key();
        Weapon* weapon = it.value();

        // -- 1. Image de l’arme sous forme de bouton
        QPushButton* iconButton = new QPushButton;
        iconButton->setIcon(QIcon(weapon->getSpritePath()));
        iconButton->setIconSize(QSize(48, 48));
        iconButton->setFixedSize(60, 60);
        iconButton->setToolTip(weapon->getName());

        connect(iconButton, &QPushButton::clicked, this, [this, slot]() {
            emit weaponSelected(slot);
            accept();
        });

        // -- 2. Nom de l’arme
        QLabel* nameLabel = new QLabel(weapon->getName());
        nameLabel->setAlignment(Qt::AlignCenter);

        // -- 3. Layout vertical contenant image + texte
        QVBoxLayout* weaponLayout = new QVBoxLayout;
        weaponLayout->addWidget(iconButton);
        weaponLayout->addWidget(nameLabel);

        // -- 4. Widget conteneur pour aligner proprement dans la grille
        QWidget* container = new QWidget;
        container->setLayout(weaponLayout);

        grid->addWidget(container, row, col);

        // organisation en grille
        if (++col == 3) { col = 0; ++row; }
    }

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->addLayout(grid);
    setLayout(mainLayout);
}
