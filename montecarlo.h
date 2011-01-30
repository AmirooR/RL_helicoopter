#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "cluster.h"
#include "episodeelement.h"
#include <vector>
#include <octave/oct.h>

class MonteCarlo
{

public:
    MonteCarlo();
    MonteCarlo(ColumnVector tr);
    computeQ(State *state, bool action);

    saveClusters();
    loadClusters();
    clustring(vector<EpisodeElement> &episode); // it's me!
    updateClusters(vector<EpisodeElement> &episode);
    actionSelection();

private:
    vector<Cluster> clusterList;
    int maxNumOfClusters = 100;
    ColumnVector threshold;
    int ignoreThreshold;        // age fasele ye state bishtar az inbarabare thereshold bood, update nakon
    float rhoAlpha;
    float rhoSigma;
};

#endif // MONTECARLO_H
