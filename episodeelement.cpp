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

ostream& operator << (ostream &o, EpisodeElement &ep)
{
    o << *(ep.getState()) << " " << ep.getAction() << " "  << ep.getReward();
    return o;
}
istream& operator >> (istream &o, EpisodeElement &ep)
{
    State *state = new State();
    int reward;
    bool action;
    o >> *state >> action >> reward;
    ep.setAction(action);
    ep.setReward(reward);
    ep.setState(state);
    return o;
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

