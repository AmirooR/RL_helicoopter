#include "copsScene.h"
#include "cops.h"
#include "barrier.h"

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>

#include <octave/oct.h>

#include <iostream>

using namespace std;

CopsScene::CopsScene()
{
    v0 = 0;
    v_x = -12;
    a_up = -500;
    a_down = 350;
    dt = 1000 / 33;
    mouseState = NONE;
    lastCrash = false;
    startTimer(dt);
}
void CopsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = CLICKED;
}
void CopsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = RELEASED;
}

void CopsScene::addItem(QGraphicsItem *item)
{
    if (item->type() == HELICOPS)
    {
        cops = item;
    }
    if (item->type() == BARRIER)
    {
        barrier = item;
    }
    QGraphicsScene::addItem(item);
}

void CopsScene::timerEvent(QTimerEvent *)
{
    float dy = 0;
    QPointF currentPos = cops->pos();
    if (this->mouseState == CLICKED)
    {
        dy = v0 * dt/1000 + 0.5 * a_up * (dt/1000) * (dt/1000);
        v0 = v0 + a_up * dt / 1000;
    }
    if (this->mouseState == RELEASED)
    {
        dy = v0 * dt/1000 + 0.5 * a_down * (dt/1000) * (dt/1000);
        v0 = v0 + a_down * dt / 1000;
    }

    if (currentPos.y() + dy > this->sceneRect().height() - cops->boundingRect().height())
    {
        cops->setPos(currentPos.x(), this->sceneRect().height() - cops->boundingRect().height());
        v0 = v0 / 100;
    }
    else if (currentPos.y() + dy < 0)
    {
        cops->setPos(currentPos.x(),0);
        v0 = v0 / 100;
    }
    cops->moveBy(0, dy);


    barrier->moveBy(v_x,0);
    currentPos = barrier->pos();
    if (currentPos.x() < 0)
    {
        float newY = qrand() % int(this->height() - barrier->boundingRect().height());
        barrier->setPos(this->width(), newY);
        lastCrash = false;
    }
    QList<QGraphicsItem *> collidItems = this->collidingItems(barrier);
    foreach (QGraphicsItem *collideItem, collidItems)
    {
        if (collideItem->type() == HELICOPS && !lastCrash)
        {
            cout << "Crashed!" << endl;
            lastCrash = true;
        }
    }

    ColumnVector vec1 = ColumnVector(2);
    vec1(1) = 123;
    vec1(0) = 534;
    ColumnVector vec2 = ColumnVector(2);
    vec2(0) = 123;
    vec2(1) = 534;
    cout << vec1 - vec2 << endl;
}

