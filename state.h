#ifndef STATE_H
#define STATE_H

#include <octave/oct.h>
#include <ostream>

using namespace std;

class State
{
public:
    State();
    State(float vX,
          float vY,
          float distUp,
          float distDown,
          float distBarrier,
          float barrierUp,
          float barrierDown);
    float getVX();
    float getVY();
    float getDistUp();
    float getDistDown();
    float getDistBarrier();
    float getBarrierUp();
    float getBarrierDown();

    float setVX(float vX);
    float setVY(float vY);
    float setDistUp(float distUp);
    float setDistDown(float distDown);
    float setDistBarrier(float distBarrier);
    float setBarrierUp(float barrierUp);
    float setBarrierDown(float barrierDown);

    ColumnVector toColumnVector();
    ColumnVector* toColumnVectorPtr();

    enum StateIndex {
        VX = 0,
        VY,
        DIST_UP,
        DIST_DOWN,
        DIST_BARRIER,
        BARRIER_UP,
        BARRIER_DOWN
    };

    friend ostream& operator << (ostream &o, State &state);
    friend istream& operator >> (istream &i, State &state);

private:
    float vX;
    float vY;
    float distUp;
    float distDown;
    float distBarrier;
    float barrierUp;
    float barrierDown;
};

#endif // STATE_H
