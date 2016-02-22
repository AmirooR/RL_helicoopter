#include <fstream>
#include "montecarlo.h"
#include "util.h"
#include <math.h>
#include <stdlib.h>

MonteCarlo::MonteCarlo()
    :maxNumOfClusters(600)
{
    rhoAlpha = 0.001;
    rhoSigma = 0.001;

    maxState = new ColumnVector(1);
    maxState->resize(7);
    (*maxState)(State::BARRIER_DOWN) = 230.0f;
    (*maxState)(State::BARRIER_UP) = 230.0f;
    (*maxState)(State::DIST_BARRIER) = 550.0f;
    (*maxState)(State::DIST_DOWN) = 263.0f;
    (*maxState)(State::DIST_UP) = 263.0f;
    (*maxState)(State::VX) = -12.0f;
    (*maxState)(State::VY) = 500.0f;

    minState = new ColumnVector(1);
    minState->resize(7);
    (*minState)(State::BARRIER_DOWN) = 0.0f;
    (*minState)(State::BARRIER_UP) = 0.0f;
    (*minState)(State::DIST_BARRIER) = -50.0f;
    (*minState)(State::DIST_DOWN) = 0.0f;
    (*minState)(State::DIST_UP) = 0.0f;
    (*minState)(State::VX) = -12.0f;
    (*minState)(State::VY) = -550.0f;

    (*maxState) = (*maxState)-(*minState);
    ignoreThreshold = 1.7;



    vY = 0;
    distUp = 50;
    distDown = 250;
    distBarrier = 550 ;
    barrierUp =168 ;
    barrierDown = 132;



}

void MonteCarlo::setThreshold(ColumnVector tr)
{
    this->threshold = tr;

}

vector<Cluster> MonteCarlo::getClusterList()
{
    return clusterList;
}

ColumnVector* MonteCarlo::normalizeMax(ColumnVector *normalMe)

{
    ColumnVector* returnMe = new ColumnVector(1);
    returnMe->resize(7);

    (*returnMe) = ((*normalMe)-(*minState));

    for(int i=0;i<7;i++)
    {
        (*returnMe)(i) = (*returnMe)(i) / (*maxState)(i);

    }
    (*returnMe)(0) = 0.0001;
    return returnMe;
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
    return c;
}

int MonteCarlo::getMaxNumOfClusters()
{
    return maxNumOfClusters;
}

bool MonteCarlo::actionSelection(State *state, float *retQ)
{
    static float eps = 0.1;
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

    // option selection
    //    if (clickQ < -100 && releasedQ < -100 && optionCount <= 0)
    //    {
    //        if (clickQ > releasedQ && !optionUp)
    //        {
    //            optionUp = true;
    //            optionDown = false;
    //        }
    //        else if (!optionDown)
    //        {
    //            optionUp = false;
    //            optionDown = true;
    //        }
    //        optionCount = 5;
    //    }

    eps = eps > 0 ? eps - 0.0001 : 0;
    if (clickQ == releasedQ)
    {
        rnd = rand() / float(RAND_MAX);
        if (rnd < .5)
            return true;
        else
            return false;
    }
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
            //            TRACE << "new cluster created!" << endl;
            ColumnVector *newMu = new ColumnVector();
            newMu->resize(7);
            if (episodeItr->getAction() == true)
            {
                ColumnVector tt = newClusterTrUp /2;
                *newMu = *stateColumnVector + tt;
                // TRACE << "state " << stateColumnVector->transpose() << endl;
            }
            else
            {
                ColumnVector tt = newClusterTrDown/2;
                ColumnVector ttt = *stateColumnVector + tt;
                *newMu = ttt;//newClusterTrDown / 2;
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

int MonteCarlo::saveEpisode(vector<EpisodeElement> *episode)
{
    static int id = 0;
    char fileName[20] = {0};
    sprintf(fileName, "episode_%d.dat", id);
    ofstream saveFile(fileName);
    vector<EpisodeElement>::iterator episodeItr = episode->begin();
    while(episodeItr != episode->end())
    {
        saveFile << *episodeItr << endl;
        episodeItr++;
    }
    saveFile.close();
    id++;
    return id;
}

vector<EpisodeElement>* MonteCarlo::loadEpisodeFromFile(char *prefix, int startId)
{
    static int id;
    char fileName[20] = {0};
    if (startId == -1)
    {
        sprintf(fileName, "%s%d.dat", prefix, id);
    }else
    {
        sprintf(fileName, "%s%d.dat", prefix, startId);
    }
    ifstream loadFile(fileName);
    vector<EpisodeElement> *retEpisode = new vector<EpisodeElement>();
    bool action;
    float reward;
    float Q;
    while (!loadFile.eof())
    {
        State *tmpState = new State();
        loadFile >> *tmpState >> action >> reward;
        EpisodeElement *episodeElement = new EpisodeElement(tmpState, action, reward, Q);
        retEpisode->push_back(*episodeElement);
    }
    loadFile.close();
    return retEpisode;
}

vector<EpisodeElement>* MonteCarlo::episodeGeneratorFromPlay(int episodeLen)
{
    static int id = 0;
    char fileName[20] = {0};
    sprintf(fileName, "episode_%d.dat", id);
    ifstream loadFile(fileName);
    State *tmpState = new State();

    TRACE << "Generating Episode from file ..." << fileName << endl;
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
    float dy;
    float Q;
    float reward;
    bool action;


    vector<EpisodeElement> *retEpisode = new vector<EpisodeElement>();
    retEpisode->reserve(episodeLen);
    loadFile >> *tmpState >> action >> reward;
    EpisodeElement *episodeElement = new EpisodeElement(tmpState, action, reward, Q);
    retEpisode->push_back(*episodeElement);

    vY = tmpState->getVY();
    vX = tmpState->getVX();
    distUp = tmpState->getDistUp();
    distDown = tmpState->getDistDown();
    distBarrier = tmpState->getDistBarrier();
    barrierUp = tmpState->getBarrierUp();
    barrierDown = tmpState->getBarrierDown();

    for(int i = 1; i < episodeLen; i++)
    {
        //TRACE << "action: " << action << endl;
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
        distBarrier += vX;
        if (distBarrier < 50)
        {
            // check crash
            if (distUp >= barrierUp && distDown >= barrierDown)
            {
                reward = -100;
            }
        }
        if (distBarrier < -50)
        {
            // create new barrier
            barrierUp = rand() % 230;
            barrierDown = 230 - barrierUp;
            distBarrier = 550;
            //     TRACE << "one barrier passed!" << endl;
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
        loadFile >> *tmpState >> action >> reward;
        EpisodeElement *newepisodeElement = new EpisodeElement(nextState, action, reward, Q);
        retEpisode->push_back(*newepisodeElement);
    }
    loadFile.close();
    id++;
    return retEpisode;
}

vector<EpisodeElement>* MonteCarlo::episodeGenerator(int episodeLen)
{
    //TRACE << "new episode ..." << endl;
    // env parameters
    float a_up = -500;
    float a_down = 350;
    float dt = 1000 / 33;

    // state vars
    float vX;
    //    static float vY = 0;
    //    static float distUp = 50;
    //    static float distDown = 250;
    //    static float distBarrier = 550 ;
    //    static float barrierUp =168 ;
    //    static float barrierDown = 132;

    //    float vY = 0;
    //    float distUp = 50;
    //    float distDown = 250;
    //    float distBarrier = 550 ;
    //    float barrierUp =168 ;
    //    float barrierDown = 132;

    vX = -12;
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
        //TRACE << "action: " << action << endl;
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
        distDown = 263 - distUp;
        distBarrier += vX;
        if (distBarrier < 120 && distBarrier > 50)
        {
            // check crash
            if ((distDown <= barrierDown + 70 && distDown >= barrierDown && !lastCrash)
                ||
                        (distUp >= barrierUp && distUp <= barrierUp + 70 && !lastCrash))
                {
                reward = -200;
                lastCrash = true;
                //                TRACE << "barrier crash!" << endl;
                //                TRACE << "distUP: " << distUp << "distDown: " << distDown << endl;
                //                TRACE << "barrierUP: " << barrierUp << "barrierDown: " << barrierDown << endl;
            }
        }
        if (distBarrier < -50)
        {
            // create new barrier
            barrierUp = rand() % 230;
            barrierDown = 230 - barrierUp;
            distBarrier = 550;
            lastCrash = false;
            //     TRACE << "one barrier passed!" << endl;
        }
        if (distUp < 10)
        {
            distUp = 10;
            vY = vY / 100;
            reward = -200;
            distDown = 263 - distUp;
            //            TRACE << "episode: Crashed!" << endl;
        }
        if (distDown < 10)
        {
            distDown = 10;
            vY = vY / 100;
            reward = -200;
            distUp = 263 - distDown;
            //            TRACE << "episode: Crashed!" << endl;
        }

        State *nextState = new State(vX, vY, distUp, distDown, distBarrier, barrierUp, barrierDown);
        action = actionSelection(nextState, &Q);
        //        if (optionUp == true && optionCount > 0)
        //        {
        //            action = true;
        //            optionCount--;
        //            cout << "OptionUp..." << optionCount << endl;
        //        }
        //        if (optionDown == true && optionCount > 0)
        //        {
        //            action = false;
        //            optionCount--;
        //            cout << "OptionDown..." << optionCount << endl;
        //        }
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
    ColumnVector *temp;
    float* grads = new float(2);
    ColumnVector s_mu = e.getState()->toColumnVector() - *(c.getMu());
    temp = normalizeMax(&s_mu);
    //  TRACE << "s_mu: " << s_mu.transpose() << endl;
    s_mu = *temp;
    RowVector s_mu_transpose = s_mu.transpose();

    float sigma2 = e.getAction() ? c.getSigmaUp() : c.getSigmaDown(); //NOTE: sigma2 is stored in clusters!
    sigma2 = fabs(sigma2);
    float s_mu_sig = (s_mu_transpose * s_mu) / (2 * sigma2);
    float namaE = expf(-s_mu_sig);
    float delta = Returns - fval;
    grads[0] = -2.0 * namaE * delta;
    float alpha = e.getAction() ? c.getAlphaUp() : c.getAlphaDown();
    grads[1] = -2.0 * alpha * (delta) * s_mu_sig * namaE / sigma2;
    //   TRACE << "Alpha: " << alpha << " Sigma2: " << sigma2 << endl;
    //   fprintf(stderr, "TRACE:: %.6f, %.6f", alpha, sigma2);
    delete temp;
    return grads;
}

void MonteCarlo::updateClusters(vector<EpisodeElement> &episode)
{
    int i;
    int numStates = episode.size();
    vector<float> qvalues;
    int Returns = 0;
    float error = 0.0f;
    for(i=0;i<numStates;i++)
    {
        EpisodeElement e = episode[i];
        Returns += e.getReward();
        qvalues.push_back(computeQ(e.getState(),e.getAction()));     //TODO: exp recomputation -> low performace
    }
    cout<<     "Returns: "   <<    Returns   << endl;

    for(i = 0; i < numStates; i++)
    {
        EpisodeElement e = episode[i];
        vector<Cluster>::iterator clusterItr = clusterList.begin();
        while(clusterItr != clusterList.end())
        {
            bool ignore = false;
            ColumnVector mvec = clusterItr->distance(e.getState()->toColumnVector());
            ColumnVector igMe = threshold * ignoreThreshold;
            ColumnVector vec = mvec - igMe;

            //            TRACE << "distance: " << clusterItr->distance(e.getState()->toColumnVector()).transpose() << endl;
            //            TRACE << "vec: " << vec.transpose() << endl;
            //            TRACE << "threshold: " << threshold.transpose() * ignoreThreshold << endl;
            for(int k=0;k<threshold.dim1();k++)
            {
                if(vec(k) >= 0)
                {
                    ignore = true;
                    break;
                }
            }

            if(!ignore) //TODO: works?
            {
                //float *grads = new float(2);
                float* grads = calcGrad(qvalues[i],e,Returns,*clusterItr);
                error += Returns - qvalues[i];
                //TRACE << "Return: " << endl;
                e.getAction() ? clusterItr->setAlphaUp(clusterItr->getAlphaUp()-(this->rhoAlpha)*grads[0]) : clusterItr->setAlphaDown(clusterItr->getAlphaDown()-(this->rhoAlpha)*grads[0]);
                e.getAction() ? clusterItr->setSigmaUp(clusterItr->getSigmaUp()-(this->rhoSigma)*grads[1]) : clusterItr->setSigmaDown(clusterItr->getSigmaDown()-(this->rhoSigma)*grads[1]);
                //          TRACE << "GradeAlpha: " << grads[0] << " GradSigma: " << grads[1] << endl;
                delete grads;
            }
            clusterItr++;
        }
        Returns -= e.getReward();
    }
    cout << "error is: " << error << endl;
}

float MonteCarlo::computeQ(State *state, bool action)
{
    ColumnVector* temp;
    float val = 0.0f;
    vector<Cluster>::iterator clusterItr = clusterList.begin();
    while(clusterItr != clusterList.end())
    {

        bool ignore = false;
        ColumnVector mvec = clusterItr->distance(state->toColumnVector());
        ColumnVector igMe = threshold * ignoreThreshold;
        ColumnVector vec = mvec - igMe;
        for(int k=0;k<threshold.dim1();k++)
        {
            if(vec(k) >= 0)
            {
                ignore = true;
                break;
            }
        }

        if(!ignore)
        {
            ColumnVector s_mu = state->toColumnVector() - *(clusterItr->getMu());
            temp = normalizeMax(&s_mu);
            s_mu = *temp;
            RowVector s_mu_transpose = s_mu.transpose();
            float sigma2 = action ? clusterItr->getSigmaUp() : clusterItr->getSigmaDown();
            sigma2 = fabs(sigma2);
            float s_mu_sig = (s_mu_transpose * s_mu)/(2*sigma2);
            float namaE = expf(-s_mu_sig);
            float alpha = action ? clusterItr->getAlphaUp() : clusterItr->getAlphaDown();
            val += alpha*namaE;
            delete temp;
        }
        clusterItr++;
    }
    //   TRACE << "Q: " << val << endl;
    return val;
}
