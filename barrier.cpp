#include "barrier.h"

Barrier::Barrier(qreal x, qreal y, qreal width, qreal height):QGraphicsRectItem(x,y,width,height)
{
}

void Barrier::timerEvent(QTimerEvent *t)
{
    QRectF r = rect();
    r.moveLeft(1);
    this->setRect(r);
}
