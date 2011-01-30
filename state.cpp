#include "state.h"

State::State()
{
}


State::State(float vX, float vY, float distUp, float distDown, float distBarrier)
          :vX(vX), vY(vY), distUp(distUp), distDown(distDown), distBarrier(distBarrier)
{

}

float State::getVX()
{
    return this->vX;
}

float State::getVY()
{
    return this->vY;
}

float State::getDistUp()
{
    return this->distUp;
}

float State::getDistDown()
{
    return this->distDown;
}

float State::getDistBarrier()
{
    return this->distBarrier;
}

float State::setVX(float vX)
{
    this->vX = vX;
}
float State::setVY(float vY)
{
    this->vY = vY;
}
float State::setDistUp(float distUp)
{
    this->distUp = distUp;
}
float State::setDistDown(float distDown)
{
    this->distDown = distDown;
}
float State::setDistBarrier(float distBarrier)
{
    this->distBarrier = distBarrier;
}


ColumnVector State::toColumnVector()
{
    ColumnVector retVec = ColumnVector(5);
    retVec(0) = vX;
    retVec(1) = vY;
    retVec(2) = distUp;
    retVec(3) = distDown;
    retVec(4) = distBarrier;
    return retVec;
}


ColumnVector* State::toColumnVectorPtr()
{
    ColumnVector *retVec = new ColumnVector(5);
    (*retVec)(0) = vX;
    (*retVec)(1) = vY;
    (*retVec)(2) = distUp;
    (*retVec)(3) = distDown;
    (*retVec)(4) = distBarrier;
    return retVec;
}
