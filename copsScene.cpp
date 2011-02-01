#include "copsScene.h"
#include "cops.h"
#include "barrier.h"
#include "util.h"
#include "episodeelement.h"

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>
#include <QPointF>

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

    ColumnVector tr(7);
    tr(State::VX) = -v_x / 3.0;
    tr(State::VY) = 1050 / 3.0;
    tr(State::DIST_UP) = 300 / 4.0;
    tr(State::DIST_DOWN) = 300 / 4.0;
    tr(State::DIST_BARRIER) = 550 / 3.0;
    tr(State::BARRIER_UP) = 200 / 4.0;
    tr(State::BARRIER_DOWN) = 200 / 4.0;

    monteCarlo.setThreshold(tr);
    cout<< "CopsScene" <<endl;
    episode = new vector<EpisodeElement>();
    episode->reserve(200);
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
    static int clusterMe = 0;
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
            TRACE << barrier->pos().x() << "," << cops->pos().x() + cops->boundingRect().width() << endl;
            lastCrash = true;
        }
    }
    clusterMe++;
    //vector<EpisodeElement> *
    if (clusterMe % 100 == 0)
    {
        cout << "New Round!" << endl;
        monteCarlo.clustring(*episode); //TODO: check? not used pointers
        cout << "Number of Clusters: " << monteCarlo.getClusterList().size() << endl;
        episode->empty();
        episode->reserve(100);
    }


    float distUp = cops->pos().y();
    float distDown = this->height() - cops->pos().y();
    float distBarrier = barrier->pos().x() - cops->pos().x();
    float barrierUp = barrier->pos().y();
    float barrierDown = this->height() - barrier->pos().y();

    State *state = new State(v_x, v0, distUp, distDown,
                             distBarrier,
                             barrierUp,
                             barrierDown);

//    TRACE << "distUp: " << distUp << " ,distDown: " << distDown << endl;
//    TRACE << "distBarrier: " << distBarrier << endl;
//    TRACE << "barrierUp: " << barrierUp << " ,barrierDown: " << barrierDown << endl;
    bool action = this->mouseState == CLICKED ? true : false;
    EpisodeElement ep(state, action, 1); //TODO: check? not used pointers
    episode->push_back(ep);
}

