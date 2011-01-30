#include "episodeelement.h"

EpisodeElement::EpisodeElement()
{
}

EpisodeElement::EpisodeElement(State *state, bool action, int reward)
    :state(state), action(action), reward(reward)
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

