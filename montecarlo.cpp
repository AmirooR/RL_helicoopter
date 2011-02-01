#include "montecarlo.h"
#include "util.h"

MonteCarlo::MonteCarlo()
    :maxNumOfClusters(50)
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

void MonteCarlo::saveClusters()
{
    TRACE<<"saving clusters..."<<endl;
    //TODO: name of file
    FILE* fp = fopen("CLUSTERS.dat","wb");
    int size = clusterList.size();
    fwrite(&size,sizeof(int),1,fp);
    Cluster c2 = clusterList[0];
    int dim = c2.getMu()->dim1();
    fwrite(&dim,sizeof(int),1,fp);
    for(int i=0;i<size;i++)
    {
        Cluster c = clusterList[i];
        c.save(fp);
    }
    fclose(fp);
}

void MonteCarlo::loadClusters()
{
    cerr<<"loading clusters!.."<<endl;
    FILE* fp = fopen("CLUSTERS.dat","rb");
    int size;
    fread(&size,sizeof(int),1,fp);
    int dims;
    fread(&dims,sizeof(int),1,fp);
    cerr<<"size is "<<size<<endl;
    cerr<<"size is "<<size<<endl;
    for(int i=0;i<size;i++)
    {
        Cluster c = loadSingleCluster(fp, dims);
        try
        {
            clusterList.push_back(c);
        }
        catch(exception e)
        {
            TRACE << e.what() << endl;
        }
    }
    TRACE<<size<<" clusters loaded"<<endl;
    fclose(fp);
}

Cluster MonteCarlo::loadSingleCluster(FILE* fp, int dim)
{
#ifdef AMIR
    cerr<<"Amir deffed!"<<endl;
    ColumnVector* mean = new ColumnVector(1);
    fread(mean,sizeof(ColumnVector),1,fp);
#else
    cerr<<"ALI!"<<endl;
    ColumnVector *mean = new ColumnVector();
    mean->resize(7);
    for(int i=0;i<dim;i++)
    {
        float x;
        fread(&x,sizeof(float),1,fp);
        (*mean)(i) = x;
    }
#endif
    float sigUp,sigDown,aUp,aDown;
    fread(&sigUp,sizeof(float),1,fp);
    fread(&aUp,sizeof(float),1,fp);
    fread(&sigDown,sizeof(float),1,fp);
    fread(&aDown,sizeof(float),1,fp);
    Cluster c;
    c.setAlphaDown(aDown);
    c.setAlphaUp(aUp);
    c.setSigmaDown(sigDown);
    c.setSigmaUp(sigUp);
    c.setMu(mean);
    TRACE << mean->transpose() << endl;
    return c;
}

int MonteCarlo::getMaxNumOfClusters()
{
    return maxNumOfClusters;
}

bool MonteCarlo::actionSelection(State *state, float *retQ)
{
    static float eps = 0.2;
    float clickQ = this->computeQ(state, true);
    float releasedQ = this->computeQ(state, false);
    float rnd = rand() / float(RAND_MAX);
    if (rnd < eps)
    {
        // min
        *retQ = min(clickQ, releasedQ);
    }
    else
    {
        // max
        *retQ = max(clickQ, releasedQ);

    }

    eps -= 0.001;
    if (*retQ == clickQ)
    {
        return true;
    }
    return false;
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
            ColumnVector *newMu = new ColumnVector();
            newMu->resize(7);
            if (episodeItr->getAction() == true)
            {
                *newMu = *stateColumnVector + newClusterTrUp / 2;
                // TRACE << "state " << stateColumnVector->transpose() << endl;
            }
            else
            {
                *newMu = *stateColumnVector + newClusterTrDown / 2;
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


vector<EpisodeElement>* MonteCarlo::episodeGenerator(int episodeLen)
{
    // env parameters
    float a_up = -500;
    float a_down = 350;
    float dt = 1000 / 33;

    // state vars
    float vX;
    float vY;
    float distUp;
    float distDown;
    float distBarrier;
    float barrierUp;
    float barrierDown;

    vX = -12;
    vY = 0;
    distUp = 50;
    distDown = 250;
    distBarrier = 550;
    barrierUp = 168;
    barrierDown = 132;
    float dy;

    vector<EpisodeElement> *retEpisode = new vector<EpisodeElement>();
    retEpisode->reserve(episodeLen);


    State *startState = new State(vX, vY, distUp, distDown, distBarrier, barrierUp, barrierDown);
    bool action;
    float Q;
    action = actionSelection(startState, &Q);
    EpisodeElement *episodeElement = new EpisodeElement(startState, action, 1, Q);
    retEpisode->push_back(*episodeElement);

    for(int i = 1; i < episodeLen; i++)
    {
        TRACE << "action: " << action << endl;
        int reward = 1;
        if (action)
        {
            dy = vY * dt/1000 + 0.5 * a_up * (dt/1000) * (dt/1000);
            vY = vY + a_up * dt / 1000;
        }
        else
        {
            dy = vY * dt/1000 + 0.5 * a_down * (dt/1000) * (dt/1000);
            vY = vY + a_down * dt / 1000;
        }
        distUp += dy;
        distDown -= dy;
        distBarrier += vX * dt/1000;
        if (distBarrier < 0)
        {
            // check crash
            if (distUp >= barrierUp && distDown >= barrierDown)
            {
                reward = -100;
            }
            // create new barrier
            barrierUp = rand() % 230;
            barrierDown = 230 - barrierUp;
            distBarrier = 550;
        }
        if (distUp <= 0)
        {
            distUp = 0;
            vY = 0;
            reward = -100;
            distDown = 263;
        }
        if (distDown <= 0)
        {
            distUp = 263;
            vY = 0;
            reward = -100;
            distDown = 0;
        }
        State *nextState = new State(vX, vY, distUp, distDown, distBarrier, barrierUp, barrierDown);
        action = actionSelection(nextState, &Q);
        EpisodeElement *newepisodeElement = new EpisodeElement(nextState, action, reward, Q);
        retEpisode->push_back(*newepisodeElement);
    }
    return retEpisode;
}


/*
 * Computing gradients of J in respect to alpha and sigma2
 * first element of return value is grad(J)/alpha
 * second element fo return value is grad(J)/sigma2
 */
float* MonteCarlo::calcGrad(float fval,EpisodeElement e,int Returns,Cluster c)
{
    float* grads = new float(2);
    ColumnVector s_mu = e.getState()->toColumnVector() - *(c.getMu());
    RowVector s_mu_transpose = s_mu.transpose();

    float sigma2 = e.getAction() ? c.getSigmaUp() : c.getSigmaDown(); //NOTE: sigma2 is stored in clusters!
    float s_mu_sig = (s_mu_transpose * s_mu) / (2 * sigma2);
    float namaE = expf(-s_mu_sig);
    float delta = Returns - fval;
    grads[0] = -2.0 * namaE * delta;
    float alpha = e.getAction() ? c.getAlphaUp() : c.getAlphaDown();
    grads[1] = -2.0 * alpha * (delta) * s_mu_sig * namaE / sigma2;
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
            bool ignore = false;
            ColumnVector vec = c.distance(e.getState()->toColumnVector()) - threshold * ignoreThreshold;
            for(int k=0;k<threshold.dim1();k++)
            {
                if(vec(k)>=0)
                {
                    ignore = true;
                    break;
                }
            }
            if(!ignore) //TODO: works?
            {
                //float *grads = new float(2);
                float* grads = calcGrad(qvalues[i],e,Returns,c);
                e.getAction() ? c.setAlphaUp(c.getAlphaUp()-(this->rhoAlpha)*grads[0]) : c.setAlphaDown(c.getAlphaDown()-(this->rhoAlpha)*grads[0]);
                e.getAction() ? c.setSigmaUp(c.getSigmaUp()-(this->rhoSigma)*grads[1]) : c.setSigmaDown(c.getSigmaDown()-(this->rhoSigma)*grads[1]);
                delete grads;
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
        bool ignore = false;
        ColumnVector vec = c.distance(state->toColumnVector()) - threshold * ignoreThreshold;
        for(int k=0;k<threshold.dim1();k++)
        {
            if(vec(k)>=0)
            {
                ignore = true;
                break;
            }
        }

        if(!ignore)
        {
            ColumnVector s_mu = state->toColumnVector() - *(c.getMu());
            RowVector s_mu_transpose = s_mu.transpose();
            float sigma2 = action ? c.getSigmaUp() : c.getSigmaDown();
            float s_mu_sig = (s_mu_transpose * s_mu)/(2*sigma2);
            float namaE = expf(-s_mu_sig);
            float alpha = action?c.getAlphaUp() : c.getAlphaDown();
            val += alpha*namaE;
        }
    }
    return val;
}
