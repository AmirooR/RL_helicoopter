#include "copsScene.h"
#include "cops.h"
#include "barrier.h"
#include "util.h"
#include "episodeelement.h"

#include <QGraphicsScene>
#include <QList>
#include <QGraphicsItem>
#include <QPointF>
#include <QKeyEvent>
#include <string>

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
    monteCarlo.loadClusters();
    cout<< "CopsScene" <<endl;
    episode = new vector<EpisodeElement>();
    episode->reserve(200);
    //timerId = startTimer(dt);
    saveEpisode = false;
    autoPilot = false;
}
void CopsScene::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = CLICKED;
}
void CopsScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    this->mouseState = RELEASED;
}
void CopsScene::keyPressEvent(QKeyEvent *event)
{
    static bool toggle = true;
    if (event->key() == ' ')
    {
        if (toggle)
        {
            killTimer(timerId);
        }
        else
        {
            timerId = startTimer(dt);
        }
        toggle = !toggle;
    }
    if (event->key() == 'N')
    {
        for (int i = 0; i < 1000; i++)
        {
            vector<EpisodeElement> *episode = this->monteCarlo.episodeGenerator(100);
            this->episodeVisualizer(episode);
            monteCarlo.updateClusters(*episode);
        }
    }
    if (event->key() == 'S') // save clusters
    {
        this->monteCarlo.saveClusters();
    }
    if (event->key() == 'P')
    {
        autoPilot = !autoPilot;
        if (autoPilot)
        {
            cerr << "Auto Pilot ..." << endl;
        }
        if (!autoPilot)
        {
            cerr << "Stop auto pilot ..." << endl;
        }
    }
    if (event->key() == 'I')
    {
        saveEpisode = !saveEpisode;
        if (saveEpisode)
        {
            cerr << "Saving Episodes ..." << endl;
        }
        if (!saveEpisode)
        {
            cerr << "Stop saving Episodes ..." << endl;
        }
    }
}

void CopsScene::episodeVisualizer(vector<EpisodeElement> *episode)
{
    vector<EpisodeElement>::iterator episodeItr = episode->begin();
    while(episodeItr != episode->end())
    {
        cops->setPos(50, episodeItr->getState()->getDistUp());
        barrier->setPos(50 + episodeItr->getState()->getDistBarrier(), episodeItr->getState()->getBarrierUp());
        usleep(330000);
        TRACE << cops->pos().y() << endl;
        episodeItr++;
    }
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
    static bool clustersSaved = true; //NOTE: make it false for saving
    static int clusterMe = 0;
    static int episodeItr = 0;
    int reward = 1;
    bool action;
    float dy = 0;
    if (!autoPilot)
    {
        QPointF currentPos = cops->pos();
        if (this->mouseState == CLICKED)
        {
            dy = v0 * dt/1000 + 0.5 * a_up * (dt/1000) * (dt/1000);
            v0 = v0 + a_up * dt / 1000;
            action = true;

        }
        if (this->mouseState == RELEASED)
        {
            dy = v0 * dt/1000 + 0.5 * a_down * (dt/1000) * (dt/1000);
            v0 = v0 + a_down * dt / 1000;
            action = false;
        }

        if (currentPos.y() + dy > this->sceneRect().height() - cops->boundingRect().height())
        {
            cops->setPos(currentPos.x(), this->sceneRect().height() - cops->boundingRect().height());
            v0 = v0 / 100;
            reward = -1;
        }
        else if (currentPos.y() + dy < 0)
        {
            cops->setPos(currentPos.x(),0);
            v0 = v0 / 100;
            reward = -1;
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
                reward = -100;
                TRACE << barrier->pos().x() << "," << cops->pos().x() + cops->boundingRect().width() << endl;
                lastCrash = true;
            }
        }
        clusterMe++;
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
        EpisodeElement ep(state, action, reward); //TODO: check? not used pointers
        episode->push_back(ep);

        if (clusterMe % 100 == 0)
        {
            cout << "New Round!" << endl;
            monteCarlo.clustring(*episode); //TODO: check? not used pointers
            monteCarlo.updateClusters(*episode);
            if (saveEpisode)
            {
                monteCarlo.saveEpisode(episode);
            }
            cout << "Number of Clusters: " << monteCarlo.getClusterList().size() << endl;
            episode->clear();
            episode->reserve(100);
            if(monteCarlo.getClusterList().size() == monteCarlo.getMaxNumOfClusters() && !clustersSaved)
            {
                monteCarlo.saveClusters();
                clustersSaved = true;
            }
        }
    }
    else
    {
        if (episodeItr % 100 == 0)
        {
            episode = this->monteCarlo.episodeGeneratorFromPlay(100);
            episodeItr = 0;
        }
        EpisodeElement ep = (*episode)[episodeItr];
        ep.getState();
        cops->setPos(50, ep.getState()->getDistUp());
        barrier->setPos(50 + ep.getState()->getDistBarrier(), ep.getState()->getBarrierUp());
        episodeItr++;
    }
}
