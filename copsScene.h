#ifndef COPS_SCENE_H
#define COPS_SCENE_H

#include <QGraphicsScene>

//! [0]
class CopsScene :  public QGraphicsScene
{
private:
    enum MouseStateEnum
    {
        CLICKED = 1,
        RELEASED = 2,
        NONE = 3
    };
    MouseStateEnum mouseState;
    float dt;
    float v0;
    float a_up;
    float a_down;
    float v_x;
    bool lastCrash;

    QGraphicsItem *cops;
    QGraphicsItem *barrier;
public:
    CopsScene();
    void addItem(QGraphicsItem *item);
    void generateEpisode();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void timerEvent(QTimerEvent *);
};

#endif

