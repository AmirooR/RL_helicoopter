#include "episodeelement.h"

EpisodeElement::EpisodeElement()
{
}

EpisodeElement::EpisodeElement(State *state, bool action, int reward)
    :state(state), action(action), reward(reward)
{

}
EpisodeElement::EpisodeElement(State *state, bool action, int reward, float Q)
    :state(state), action(action), reward(reward), Q(Q)
{

}


State* EpisodeElement::getState()
{
        return this->state;
}
bool EpisodeElement::getAction()
{
    return this->action;
}
int EpisodeElement::getReward()
{
    return this->reward;
}
float EpisodeElement::getQ()
{
    return this->Q;
}

void EpisodeElement::setState(State *state)
{
    this->state = state;
}
void EpisodeElement::setAction(bool action)
{
    this->action = action;
}
void EpisodeElement::setReward(int reward)
{
    this->reward = reward;
}
void EpisodeElement::setQ(float Q)
{
    this->Q = Q;
}

