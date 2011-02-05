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
    bool actionSelection(State *state, float *retQ);
    vector<Cluster> getClusterList();
    vector<EpisodeElement>* episodeGenerator(int episodeLen);
    vector<EpisodeElement>* episodeGeneratorFromPlay(int episodeLen);
    vector<EpisodeElement>* loadEpisodeFromFile(char *prefix, int startId = -1);
    int getMaxNumOfClusters();
    Cluster loadSingleCluster(FILE* fp, int dim);
    int saveEpisode(vector<EpisodeElement> *episode);

    ColumnVector* normalizeMax(ColumnVector*);


private:
    vector<Cluster> clusterList;
    int maxNumOfClusters;
    ColumnVector threshold;

    float* calcGrad(float fval,EpisodeElement e,int Returns,Cluster c);

    int ignoreThreshold;        // age fasele ye state bishtar az inbarabare thereshold bood, update nakon
    float rhoAlpha;
    float rhoSigma;

    ColumnVector* maxState;
    ColumnVector* minState;





    float vY;
    float distUp;
    float distDown;
    float distBarrier;
    float barrierUp;
    float barrierDown;
    bool lastCrash;


    bool optionUp;
    bool optionDown;
    int optionCount;

};

#endif // MONTECARLO_H
