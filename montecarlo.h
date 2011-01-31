#ifndef MONTECARLO_H
#define MONTECARLO_H

#include "cluster.h"
#include "episodeelement.h"

#include <vector>
#include <octave/oct.h>

using namespace std;

class MonteCarlo
{

public:
    MonteCarlo();
    MonteCarlo(ColumnVector tr);
    float computeQ(State *state, bool action);
    void saveClusters();
    void loadClusters();
    void clustring(vector<EpisodeElement> &episode); // it's me!
    void updateClusters(vector<EpisodeElement> &episode);
    bool actionSelection();

private:
    vector<Cluster> clusterList;
    int maxNumOfClusters;
    ColumnVector threshold;
    int ignoreThreshold;        // age fasele ye state bishtar az inbarabare thereshold bood, update nakon
    float rhoAlpha;
    float rhoSigma;
};

#endif // MONTECARLO_H
