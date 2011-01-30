#ifndef EPISODEELEMENT_H
#define EPISODEELEMENT_H

#include "state.h"

class EpisodeElement
{
public:
    EpisodeElement();
    EpisodeElement(State *state, bool action, int reward);

    State *getState();
    bool getAction();
    int getReward();
    void setState(State *state);
    void setAction(bool);
    void setReward(int);

private:
    State *state;
    bool action;
    int reward;
};

#endif // EPISODEELEMENT_H
