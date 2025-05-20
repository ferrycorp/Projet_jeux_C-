#pragma once

#include <QGraphicsPixmapItem>
#include <QTimer>
#include <QObject>
#include <QVector>

class Mobm : public QObject, public QGraphicsPixmapItem {
    Q_OBJECT

public:
    Mobm(QPointF pos, int tileSize, QGraphicsItem* parent = nullptr);

    private slots:
        void nextFrame();

private:
    QVector<QPixmap> frames;
    int currentFrame = 0;
    QTimer* timer;
};