#include "mobmort.h"
#include <QGraphicsScene>

Mobm::Mobm(QPointF pos, int tileSize, QGraphicsItem* parent)
    : QGraphicsPixmapItem(parent) {

    for (int i = 1; i <= 4; ++i) {
        frames.append(QPixmap(QString("../images/mobmort_%1.png").arg(i)).scaled(tileSize, tileSize));
    }

    setPixmap(frames[0]);
    setPos(pos);

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Mobm::nextFrame);
    timer->start(100);
}

void Mobm::nextFrame() {
    currentFrame++;
    if (currentFrame >= frames.size()) {
        if (scene()) {
            scene()->removeItem(this);
        }
        deleteLater();
        return;
    }

    setPixmap(frames[currentFrame]);
}