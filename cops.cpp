#include "cops.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include <math.h>
#include <iostream>

using namespace std;



Cops::Cops() 
{
	pic = new QPixmap(":/images/cops.jpg");
        picSize = pic->size();
}

QRectF Cops::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(0, 0,
                  picSize.width(), picSize.height());
}

QPainterPath Cops::shape() const
{
    QPainterPath path;
    path.addRect(0, 0, picSize.width(), picSize.height());
    return path;
}

void Cops::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->drawPixmap(0, 0, *pic);
}
