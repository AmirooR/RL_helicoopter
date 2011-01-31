#ifndef CLUSTER_H
#define CLUSTER_H

#include <octave/oct.h>

class Cluster
{
public:
    Cluster();
    Cluster(ColumnVector mu, ColumnVector sigma, ColumnVector alpha, ColumnVector sigmaDown, ColumnVector alphaDown);
    Cluster(ColumnVector mu);
    ColumnVector getMu();
    ColumnVector getSigmaUp();
    ColumnVector getAlphaUp();
    ColumnVector getSigmaDown();
    ColumnVector getAlphaDown();

    void setMu(ColumnVector);
    void setSigmaUp(ColumnVector);
    void setAlphaUp(ColumnVector);
    void setSigmaDown(ColumnVector);
    void setAlphaDown(ColumnVector);

    bool isInCluster(ColumnVector &tr, ColumnVector &state);  // TODO: performance killer! (global tr)
    ColumnVector distance(ColumnVector point);   // calc distance of point from current cluster, return abs

private:
    ColumnVector mu;
    ColumnVector sigmaUp;
    ColumnVector alphaUp;
    ColumnVector sigmaDown;
    ColumnVector alphaDown;
};

#endif // CLUSTER_H
