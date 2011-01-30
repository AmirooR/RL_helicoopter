#ifndef STATE_H
#define STATE_H

#include <octave/oct.h>

class State
{
public:
    State();
    State(float vX,
          float vY,
          float distUp,
          float distDown,
          float distBarrier);
    float getVX();
    float getVY();
    float getDistUp();
    float getDistDown();
    float getDistBarrier();

    float setVX(float vX);
    float setVY(float vY);
    float setDistUp(float distUp);
    float setDistDown(float distDown);
    float setDistBarrier(float distBarrier);

    ColumnVector toColumnVector();
    ColumnVector* toColumnVectorPtr();



private:
    float vX;
    float vY;
    float distUp;
    float distDown;
    float distBarrier;
};

#endif // STATE_H
