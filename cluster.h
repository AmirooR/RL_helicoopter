#ifndef CLUSTER_H
#define CLUSTER_H

#include <octave/oct.h>

class Cluster
{
public:
    Cluster();
    Cluster(ColumnVector mu, float sigma, float alpha, float sigmaDown, float alphaDown);
    Cluster(ColumnVector mu);
    ColumnVector getMu();
    float getSigmaUp();
    float getAlphaUp();
    float getSigmaDown();
    float getAlphaDown();

    void setMu(ColumnVector);
    void setSigmaUp(float);
    void setAlphaUp(float);
    void setSigmaDown(float);
    void setAlphaDown(float);

    bool isInCluster(ColumnVector &tr, ColumnVector &state);  // TODO: performance killer! (global tr)
    ColumnVector distance(ColumnVector point);   // calc distance of point from current cluster, return abs

private:
    ColumnVector mu;
    float sigmaUp;
    float alphaUp;
    float sigmaDown;
    float alphaDown;
};

#endif // CLUSTER_H
