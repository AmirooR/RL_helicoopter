#include "cluster.h"
#include "util.h"

using namespace std;

Cluster::Cluster()
{
    for (int i = 0; i < mu.dim1(); i++)
    {
        mu(i) = 1;
    }
}

Cluster::Cluster(ColumnVector mu)
{
    this->mu = mu;
    TRACE << "new cluster created! mu = " << mu.transpose() << endl;
}

Cluster::Cluster(ColumnVector mu, float sigma, float alpha, float sigmaDown, float alphaDown)
{    
    this->mu = mu;
    this->sigmaUp = sigma;
    this->alphaUp = alpha;
    this->sigmaDown = sigmaDown;
    this->alphaDown = alphaDown;

}

ColumnVector Cluster::getMu()
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

void Cluster::setMu(ColumnVector mu)
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

ColumnVector Cluster::distance(ColumnVector point)
{
    if (point.dim1() != this->mu.dim1())
    {
        TRACE << "size mismatch" << endl;
        return point;
    }

    ColumnVector retColumn = point - this->mu;
    for (int i = 0; i < point.dim1(); i++)
    {
        retColumn(i) = abs(retColumn(i));
    }
    return retColumn;
}

bool Cluster::isInCluster(ColumnVector &tr, ColumnVector &state)
{
    ColumnVector dist = distance(state);
    // TRACE << "dist: " << dist.transpose() << " tr: " << tr.transpose() << endl;
    for (int i = 0; i < tr.dim1(); i++)
    {
        if (dist(i) > tr(i))
        {
            return false;
        }
    }
    return true;
}
