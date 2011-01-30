#ifndef CLUSTER_H
#define CLUSTER_H

#include <octave/oct.h>

class Cluster
{
public:
    Cluster();
    Cluster(ColumnVector mu, ColumnVector sigma, ColumnVector alpha);
    ColumnVector getMu();
    ColumnVector getSigma();
    ColumnVector getAlpha();

    void setMu(ColumnVector);
    void setSigma(ColumnVector);
    void setAlpha(ColumnVector);

    bool isInCluster(ColumnVector tr, ColumnVector state);  // TODO: performance killer! (global tr)
    ColumnVector distance(ColumnVector point);   // calc distance of point from current cluster, return abs

private:
    ColumnVector mu;
    ColumnVector sigma;
    ColumnVector alpha;
};

#endif // CLUSTER_H
