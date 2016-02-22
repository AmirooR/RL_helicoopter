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

//#include <octave/oct.h>
#include "ColumnVector.h"
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
    tr(State::VY) = 1050 / 10.0;
    tr(State::DIST_UP) = 263 / 7.0;
    tr(State::DIST_DOWN) = 263 / 7.0;
    tr(State::DIST_BARRIER) = 550 / 2.0;
    tr(State::BARRIER_UP) = 230 / 4.0;
    tr(State::BARRIER_DOWN) = 230 / 4.0;

    monteCarlo.setThreshold(tr);
    episode = new vector<EpisodeElement>();
    episode->reserve(200);
    saveEpisode = false;
    autoPilot = false;
    replay = false;
    clustersSaved = true; //NOTE: make it false for saving
    pause = false;

    episodeNumber = 0;
    episodeItr = 0;
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
    if (event->key() == ' ')
    {
        if (pause)
        {
            killTimer(timerId);
        }
        else
        {
            timerId = startTimer(dt);
        }
        pause = !pause;
    }

    if (event->key() == 'S') // save clusters
    {
        cerr << "Saving Clusters ..." << endl;
        this->monteCarlo.saveClusters();
        cerr << monteCarlo.getClusterList().size() << " of clusters saved!" << endl;
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
    if (event->key() == 'L')    // load clusters
    {
        monteCarlo.loadClusters();
    }
    if (event->key() == 'G')    // Learn in Background!
    {
        for (int i = 0; i < 10000; i++)
        {
            episode = this->monteCarlo.episodeGenerator(100);
            monteCarlo.clustring(*episode);
            monteCarlo.updateClusters(*episode);
            if (saveEpisode)
            {
                monteCarlo.saveEpisode(episode);
            }
            cout << "Cluster Size: " << monteCarlo.getClusterList().size() << endl;
            cout << "Episode Number: " << episodeNumber++ << endl;
            episode->clear();
            episode->reserve(100);
        }
        TRACE << "Learning Finished!" << endl;
    }
    if (event->key() == 'R')
    {
        cout << "Replay ..." << endl;
        replay = true;
    }
    if (event->key() == 16777236) // forward
    {
        replayId += 1000;
        replayId = replayId >= 9000 ? 9000 : replayId;
        episodeItr = 0;
        replayId -= replayId % 1000;
    }
    if (event->key() == 16777234)
    {
        replayId -= 1000;
        replayId = replayId <= 0 ? 0 : replayId;
        episodeItr = 0;
        replayId -= replayId % 1000;
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
    static int clusterMe = 0;
    int reward = 1;
    bool action;
    float dy = 0;
    if (!autoPilot && !replay)
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
            reward = -100;
        }
        else if (currentPos.y() + dy < 0)
        {
            cops->setPos(currentPos.x(),0);
            v0 = v0 / 100;
            reward = -100;
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
                TRACE << cops->boundingRect().width() << endl;
                lastCrash = true;
            }
        }
        clusterMe++;
        float distUp = cops->pos().y();
        float distDown = this->height() - cops->pos().y();
        float distBarrier = barrier->pos().x() - cops->pos().x();
        float barrierUp = barrier->pos().y();
        float barrierDown = this->height() - barrier->pos().y() - barrier->boundingRect().height();

        State *state = new State(v_x, v0, distUp, distDown,
                                 distBarrier,
                                 barrierUp,
                                 barrierDown);

        //    TRACE << "distUp: " << distUp << " ,distDown: " << distDown << endl;
        //    TRACE << "distBarrier: " << distBarrier << endl;
        //        TRACE << "barrierUp: " << barrierUp << " ,barrierDown: " << barrierDown << endl;
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
            TRACE << "Number of Clusters: " << monteCarlo.getClusterList().size() << endl;
            episode->clear();
            episode->reserve(100);
            if(monteCarlo.getClusterList().size() == monteCarlo.getMaxNumOfClusters() && !clustersSaved)
            {
                monteCarlo.saveClusters();
                clustersSaved = true;
            }
        }
    }
    else if (autoPilot && !replay)        // Auto Pilot
    {
        if (episodeItr % 100 == 0)
        {
            episode->clear();

            episode->reserve(100);
            episodeItr = 0;
            episode = this->monteCarlo.episodeGenerator(100);
            //      monteCarlo.clustring(*episode);
            //    monteCarlo.updateClusters(*episode);
            //      cout << "Cluster Size: " << monteCarlo.getClusterList().size() << endl;
            //     cout << "Episode Number: " << episodeNumber++ << endl;

        }

        EpisodeElement ep = (*episode)[episodeItr];
        State *state = ep.getState();
        cops->setPos(50, state->getDistUp());
        barrier->setPos(state->getDistBarrier(), state->getBarrierUp());
        //        QList<QGraphicsItem *> collidItems = this->collidingItems(barrier);
        //        foreach (QGraphicsItem *collideItem, collidItems)
        //        {
        //            if (collideItem->type() == HELICOPS)
        //            {
        //                cout << "Crashed!" << endl;
        //                killTimer(timerId);
        //                pause = false;
        //                TRACE << ep.getQ() << endl;
        ////                TRACE << "Reward: " << ep.getReward() << endl;
        ////                cout << "distUP: " << ep.getState()->getDistUp() << " distDown: " << ep.getState()->getDistDown() << endl;
        ////                cout << "barrierUP: " << ep.getState()->getBarrierUp() << " barrierDown: " << ep.getState()->getBarrierDown() << endl;

        //            }
        //        }

        episodeItr++;
    }
    if (replay)
    {
        if (episodeItr % 100 == 0)
        {
            episode->clear();
            episode->reserve(100);
            episodeItr = 0;
            episode = monteCarlo.loadEpisodeFromFile("./episodes/episode_", replayId);
            TRACE << replayId << endl;
            replayId++;
        }

        EpisodeElement ep = (*episode)[episodeItr];
        State *state = ep.getState();
        cops->setPos(50, state->getDistUp());
        barrier->setPos(state->getDistBarrier(), state->getBarrierUp());
        episodeItr++;
    }
}
