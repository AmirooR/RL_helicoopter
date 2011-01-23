#include "cops.h"

#include <QGraphicsScene>
#include <QPainter>
#include <QStyleOption>

#include <math.h>
#include <iostream>

using namespace std;

static const double Pi = 3.14159265358979323846264338327950288419717;
static double TwoPi = 2.0 * Pi;

static qreal normalizeAngle(qreal angle)
{
    while (angle < 0)
        angle += TwoPi;
    while (angle > TwoPi)
        angle -= TwoPi;
    return angle;
}

//! [0]
Cops::Cops()
    : angle(0), speed(0), mouseEyeDirection(0),
      color(qrand() % 256, qrand() % 256, qrand() % 256)
{
	pic = new QPixmap(":/images/cops.jpg");
//   startTimer(1000 / 33);
}
//! [0]

//! [1]
QRectF Cops::boundingRect() const
{
    qreal adjust = 0.5;
    return QRectF(-20 - adjust, -20 - adjust,
                  36 + adjust, 20 + adjust);
}
//! [1]

//! [2]
QPainterPath Cops::shape() const
{
    QPainterPath path;
    path.addRect(-10, -20, 20, 40);
    return path;
}
//! [2]

//! [3]
void Cops::paint(QPainter *painter, const QStyleOptionGraphicsItem *, QWidget *)
{
    painter->setBrush(Qt::white);
    painter->drawEllipse(-10, -17, 8, 8);
    painter->drawEllipse(2, -17, 8, 8);
	cout << "paint called!" << endl;
	painter->drawPixmap(0, 0, *pic);
}
//! [3]

void Cops::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
	cout << "Clicked!" << endl;
}
//! [4]
void Cops::timerEvent(QTimerEvent *)
{
//! [4]
    // Don't move too far away
//! [5]
    QLineF lineToCenter(QPointF(0, 0), mapFromScene(0, 0));
    if (lineToCenter.length() > 150) {
        qreal angleToCenter = ::acos(lineToCenter.dx() / lineToCenter.length());
        if (lineToCenter.dy() < 0)
            angleToCenter = TwoPi - angleToCenter;
        angleToCenter = normalizeAngle((Pi - angleToCenter) + Pi / 2);

        if (angleToCenter < Pi && angleToCenter > Pi / 4) {
            // Rotate left
            angle += (angle < -Pi / 2) ? 0.25 : -0.25;
        } else if (angleToCenter >= Pi && angleToCenter < (Pi + Pi / 2 + Pi / 4)) {
            // Rotate right
            angle += (angle < Pi / 2) ? 0.25 : -0.25;
        }
    } else if (::sin(angle) < 0) {
        angle += 0.25;
    } else if (::sin(angle) > 0) {
        angle -= 0.25;
//! [5] //! [6]
    }
//! [6]

    // Try not to crash with any other mice
//! [7]
    QList<QGraphicsItem *> dangerMice = scene()->items(QPolygonF()
                                                       << mapToScene(0, 0)
                                                       << mapToScene(-30, -50)
                                                       << mapToScene(30, -50));
    foreach (QGraphicsItem *item, dangerMice) {
        if (item == this)
            continue;
        
        QLineF lineToCops(QPointF(0, 0), mapFromItem(item, 0, 0));
        qreal angleToCops = ::acos(lineToCops.dx() / lineToCops.length());
        if (lineToCops.dy() < 0)
            angleToCops = TwoPi - angleToCops;
        angleToCops = normalizeAngle((Pi - angleToCops) + Pi / 2);

        if (angleToCops >= 0 && angleToCops < Pi / 2) {
            // Rotate right
            angle += 0.5;
        } else if (angleToCops <= TwoPi && angleToCops > (TwoPi - Pi / 2)) {
            // Rotate left
            angle -= 0.5;
//! [7] //! [8]
        }
//! [8] //! [9]
    }
//! [9]

    // Add some random movement
//! [10]
    if (dangerMice.size() > 1 && (qrand() % 10) == 0) {
        if (qrand() % 1)
            angle += (qrand() % 100) / 500.0;
        else
            angle -= (qrand() % 100) / 500.0;
    }
//! [10]

//! [11]
    speed += (-50 + qrand() % 100) / 100.0;

    qreal dx = ::sin(angle) * 10;
    mouseEyeDirection = (qAbs(dx / 5) < 1) ? 0 : dx / 5;

    rotate(dx);
    setPos(mapToParent(0, -(3 + sin(speed) * 3)));
}
//! [11]
