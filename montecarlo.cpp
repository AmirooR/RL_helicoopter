#include "montecarlo.h"
#include "util.h"

MonteCarlo::MonteCarlo()
    :maxNumOfClusters(150)
{
}

void MonteCarlo::setThreshold(ColumnVector tr)
{
    this->threshold = tr;
}

vector<Cluster> MonteCarlo::getClusterList()
{
    return clusterList;
}

void MonteCarlo::clustring(vector<EpisodeElement> &episode)
{
    if (clusterList.size() >= maxNumOfClusters)
    {
        return;
    }
    static int orphanState = 0;
    ColumnVector newClusterTrDown = this->threshold;
    newClusterTrDown(State::DIST_DOWN) = -newClusterTrDown(State::DIST_DOWN);

    ColumnVector newClusterTrUp = this->threshold;
    newClusterTrUp(State::VY) = -newClusterTrUp(State::VY);
    newClusterTrUp(State::DIST_UP) = -newClusterTrUp(State::DIST_UP);

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
                break;
            }
            clusterItr++;
        }
        // no match cluster, create new cluster
        if (!isInAnyCluster && clusterList.size() < maxNumOfClusters)
        {
            ColumnVector newMu;
            if (episodeItr->getAction() == true)
            {
                newMu = *stateColumnVector + newClusterTrUp / 2;
                // TRACE << "state " << stateColumnVector->transpose() << endl;
            }
            else
            {
                newMu = *stateColumnVector + newClusterTrDown / 2;
                // TRACE << "state " << stateColumnVector->transpose() << endl;
            }
            Cluster newCluster(newMu);
            clusterList.push_back(newCluster);
        }
        if (!isInAnyCluster && clusterList.size() >= maxNumOfClusters)
        {
            TRACE << "Orphan state!" << orphanState++ << endl;
        }
        episodeItr++;
    }
}


vector<EpisodeElement> & generateDummyEpisode(int episodeLen)
{
    // env parameters
    float v0 = 0;
    float v_x = -12;
    float a_up = -500;
    float a_down = 350;
    float dt = 1000 / 33;

    vector<EpisodeElement> *retEpisode = new vector<EpisodeElement>();
    retEpisode->reserve(episodeLen);

    State *startState = new State(v_x, v0, 50, 250, 500, 50, 60);
    EpisodeElement *episodeElement = new EpisodeElement(startState, 0, 1);
    retEpisode->push_back(*episodeElement);
    for(int i = 0; i < episodeLen; i++)
    {
        //State nextState = new State();
        //(*retEpisode)[0] = 324;
    }
    return *retEpisode;
}



/*
 * Computing gradients of J in respect to alpha and sigma2
 * first element of return value is grad(J)/alpha
 * second element fo return value is grad(J)/sigma2
 */
float[] MonteCarlo::calcGrad(float fval,EpisodeElement e,int Returns,Cluster c)
{
    float grads[2];
    ColumnVector s_mu = e.getState()->toColumnVector()-c.getMu();
    RowVector s_mu_transpose = s_mu.transpose();
    float sigma2 = c.getSigma(1); //NOTE: sigma2 is stored in clusters!
    float s_mu_sig = (s_mu_transpose * s_mu)/(2*sigma2); //TODO: correct for each action
    float namaE = expf(-s_mu_sig);
    float delta = Returns - fval;
    grads[0] = -2.0*namaE*delta;
    grads[1] = -2.0*c.getAlpha()*(delta)*s_mu_sig*nameE/sigma2;
    return grads;
}

void MonteCarlo::updateClusters(vector<EpisodeElement> &episode)
{
    int i,j;
    int numStates = episode.size();
    vector<float> qvalues;
    int Returns = 0;
    for(i=0;i<numStates;i++)
    {
        EpisodeElement e = episode[i];
        Returns += e.getReward();
        qvalues.push_back(computeQ(e.getState(),e.getAction()));     //TODO: exp recomputation -> low performace
    }

    for(i=0;i<numStates;i++)
    {
        EpisodeElement e = episode[i];
        for(j=0;j<clusterList.size();j++)
        {
            Cluster c = clusterList[j];
            if(c.distance(e.getState()->toColumnVector()) < ignoreThreshold * threshold) //TODO: works?
            {
                float grads[] = calcGrad(qvalues[i],e,Returns,c);
                c.setAlpha(c.getAlpha()-(this->rhoAlpha)*grads[0]);
                c.setSigma(c.getSigma()-(this->rhoSigma*grads[1]));     //TODO: sigma should be a scalar instead of vector
            }
        }
        Returns -= e.getReward();
    }
}

float MonteCarlo::computeQ(State *state, bool action)
{
    float val = 0.0f;
    for(int i=0;i<clusterList.size();i++)
    {
        Cluster c= clusterList[i];
        //TODO: if(distance>thresh) ?
        ColumnVector s_mu = state->toColumnVector()-c.getMu();
        RowVector s_mu_transpose = s_mu.transpose();
        float sigma2 = c.getSigma(1); //NOTE: sigma2 is stored in clusters!
        float s_mu_sig = (s_mu_transpose * s_mu)/(2*sigma2); //TODO: correct for each action
        float namaE = expf(-s_mu_sig);
        val += c.getAlpha()*namaE;
    }
    return val;
}
