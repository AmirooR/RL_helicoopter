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
    void setThreshold(ColumnVector tr);
    float computeQ(State *state, bool action);
    void saveClusters();
    void loadClusters();
    void clustring(vector<EpisodeElement> &episode); // it's me!
    void updateClusters(vector<EpisodeElement> &episode);
    bool actionSelection();
    vector<Cluster> getClusterList();
private:
    vector<Cluster> clusterList;
    int maxNumOfClusters;
    ColumnVector threshold;

    float[] calcGrad(float fval,EpisodeElement e,int Returns,Cluster c);

    int ignoreThreshold;        // age fasele ye state bishtar az inbarabare thereshold bood, update nakon
    float rhoAlpha;
    float rhoSigma;

    vector<EpisodeElement>& generateDummyEpisode();
};

#endif // MONTECARLO_H
