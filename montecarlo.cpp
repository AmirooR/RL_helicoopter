#include "montecarlo.h"

MonteCarlo::MonteCarlo()
        :maxNumOfClusters(100)
{
}

void MonteCarlo::clustring(vector<EpisodeElement> &episode)
{
    vector<EpisodeElement>::iterator episodeItr = episode.begin();
    while(episodeItr != episode.end())
    {
        ColumnVector *stateColumnVector = episodeItr->getState()->toColumnVectorPtr();
        vector<Cluster>::iterator clusterItr = clusterList.begin();
        bool isInAnyCluster = false;
        while(clusterItr != clusterList.end())
        {
            if (clusterItr->isInCluster(this->threshold, *stateColumnVector))
            {
                isInAnyCluster = true;
            }
        }
        // no match cluster
        // create new cluster
        Cluster newCluster;
        clusterList.push_back(newCluster);
    }
}
