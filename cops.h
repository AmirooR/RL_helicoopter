#ifndef COPS_H
#define COPS_H

#include <QGraphicsItem>
#include <QObject>
#include <QPixmap>

#define HELICOPS 65537
//! [0]
class Cops : public QObject, public QGraphicsItem
{
    Q_OBJECT

public:
    enum { Type = HELICOPS };
    int type() const
    {
        return Type;
    }

    Cops();
    QRectF boundingRect() const;
    QPainterPath shape() const;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option,
               QWidget *widget);

protected:

private:
    QPixmap *pic;
    QSize picSize;
};

#endif
