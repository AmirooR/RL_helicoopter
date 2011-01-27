#ifndef BARRIER_H
#define BARRIER_H

#include <QGraphicsRectItem>
#define BARRIER 65538
class Barrier: public QGraphicsRectItem
{

public:
    enum { Type = BARRIER };
    int type() const
    {
        return Type;
    }
    Barrier(qreal x, qreal y, qreal width, qreal height);

protected:
    void timerEvent(QTimerEvent *);
};

#endif // BARRIER_H
