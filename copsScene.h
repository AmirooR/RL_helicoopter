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
        RELEASED = 2
    };
    MouseStateEnum mouseState;
public:
    CopsScene();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent *event);
    void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    void timerEvent(QTimerEvent *);
};

#endif

