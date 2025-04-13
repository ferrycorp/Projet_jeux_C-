#ifndef PLAYER_H
#define PLAYER_H

#include <QGraphicsPixmapItem>
#include <QKeyEvent>
#include <QObject>
#include <QTimer>
#include <QVector>

class Player : public QObject, public QGraphicsPixmapItem {
Q_OBJECT

public:
    Player(QGraphicsItem *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void keyReleaseEvent(QKeyEvent* event) override;

private slots:

    void nextFrame();

private:
    QVector<QPixmap> walkFrames;
    int currentFrame;
    QTimer *animationTimer;
    int step;
    qreal rotationAngle;// 0 = bas, 90 = gauche, etc.
    bool moving = false;
    bool keyUp = false;
    bool keyDown = false;
    bool keyLeft = false;
    bool keyRight = false;
};

#endif // PLAYER_H
