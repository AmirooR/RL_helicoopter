#include "copsScene.h"
#include "cops.h"

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>

#include <iostream>

using namespace std;

CopsScene::CopsScene()
{
    startTimer(1000 / 33);
}
void CopsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = CLICKED;
}
void CopsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = RELEASED;
}

void CopsScene::timerEvent(QTimerEvent *)
{
    cout << "Scene Clicked!" << endl;
    QList<QGraphicsItem *> graphiItems = this->items();
    foreach (QGraphicsItem *item, graphiItems) 
    {
        cout << "Item type is: " << item->type() << endl;
        if (this->mouseState == CLICKED)
        {
            item->moveBy(0,-2);
        }
        if (this->mouseState == RELEASED)
        {
            item->moveBy(0,2);
        }
    }
}
