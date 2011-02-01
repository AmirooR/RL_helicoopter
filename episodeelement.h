#ifndef EPISODEELEMENT_H
#define EPISODEELEMENT_H

#include "state.h"

class EpisodeElement
{
public:
    EpisodeElement();
    EpisodeElement(State *state, bool action, int reward);
    EpisodeElement(State *state, bool action, int reward, float Q);

    State *getState();
    bool getAction();
    int getReward();
    void setState(State *state);
    void setAction(bool);
    void setReward(int);
    float getQ();
    void setQ(float Q);

private:
    State *state;
    bool action;
    int reward;
    float Q;        // Q of selected action in state!
};

#endif // EPISODEELEMENT_H
