#include "cluster.h"
#include "util.h"
#include <math.h>
#include <stdio.h>

using namespace std;

Cluster::Cluster()
{
    mu = new ColumnVector;
    mu->resize(7);
    for (int i = 0; i < mu->dim1(); i++)
    {
        (*mu)(i) = 1;
    }
    this->alphaDown = 1;
    this->alphaUp = 1;
    this->sigmaDown = 1;
    this->sigmaUp = 1;
}

Cluster::Cluster(ColumnVector *mu)
{
    this->mu = mu;
    this->alphaDown = 1;
    this->alphaUp = 1;
    this->sigmaDown = 1;
    this->sigmaUp = 1;
}

Cluster::Cluster(ColumnVector *mu, float sigma, float alpha, float sigmaDown, float alphaDown)
{
    TRACE << mu->dim1() << endl;
    this->mu = mu;
    this->sigmaUp = sigma;
    this->alphaUp = alpha;
    this->sigmaDown = sigmaDown;
    this->alphaDown = alphaDown;
}

ColumnVector* Cluster::getMu()
{
    return this->mu;
}
float Cluster::getSigmaUp()
{
    return this->sigmaUp;
}

float Cluster::getAlphaUp()
{
    return this->alphaUp;
}
float Cluster::getSigmaDown()
{
    return this->sigmaDown;
}

float Cluster::getAlphaDown()
{
    return this->alphaDown;
}

void Cluster::setMu(ColumnVector *mu)
{
    this->mu = mu;
}

void Cluster::setSigmaUp(float sigma)
{
    this->sigmaUp = sigma;
}

void Cluster::setAlphaUp(float alpha)
{
    this->alphaUp = alpha;
}
void Cluster::setSigmaDown(float sigma)
{
    this->sigmaDown = sigma;
}

void Cluster::setAlphaDown(float alpha)
{
    this->alphaDown = alpha;
}

void Cluster::save(FILE* fp)
{
#ifdef AMIR
    fwrite(this->mu,sizeof(ColumnVector),1,fp);
#else
    for(int i=0;i < mu->dim1();i++)
    {
        float x = (*mu)(i);
        fwrite(&x,sizeof(float),1,fp);
    }
#endif
    fwrite(&sigmaUp,sizeof(float),1,fp);
    fwrite(&alphaUp,sizeof(float),1,fp);
    fwrite(&sigmaDown,sizeof(float),1,fp);
    fwrite(&alphaDown,sizeof(float),1,fp);
}

ColumnVector Cluster::distance(ColumnVector point)
{
    if (point.dim1() != this->mu->dim1())
    {
        TRACE << "size mismatch: point:" << point.dim1() << "muDim:" << mu->dim1() << endl;
        return point;
    }

    ColumnVector retColumn = point - *(this->mu);
    for (int i = 0; i < point.dim1(); i++)
    {
        retColumn(i) = fabs(retColumn(i));
        //TRACE << "["<<i<< "] point(i): " << point(i) << ", mu(i): " << (*(this->mu))(i) << ", diff: " << retColumn(i) << endl;
    }
    return retColumn;
}

bool Cluster::isInCluster(ColumnVector &tr, ColumnVector &state)
{
    ColumnVector dist = distance(state);
    // TRACE << "dist: " << dist.transpose() << " tr: " << tr.transpose() << endl;
    //TRACE << "tr dim: "<<tr.dim1() << endl;
    for (int i = 0; i < tr.dim1(); i++)
    {
        //TRACE << "["<<i<<"] "<<dist(i) << " > " << tr(i) << endl;
        if (dist(i) > tr(i))
        {
            return false;
        }
    }
    return true;
}
