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

Cluster::Cluster(ColumnVector mu, ColumnVector sigma, ColumnVector alpha, ColumnVector sigmaDown, ColumnVector alphaDown)
{
    if (mu.dim1() == sigma.dim1() && alpha.dim1() == mu.dim1())
    {
        this->mu = mu;
        this->sigmaUp = sigma;
        this->alphaUp = alpha;
        this->sigmaDown = sigmaDown;
        this->alphaDown = alphaDown;
    }
    else
    {
        cerr << "size mismatch" << endl;
    }
}

ColumnVector Cluster::getMu()
{
    return this->mu;
}
ColumnVector Cluster::getSigmaUp()
{
    return this->sigmaUp;
}

ColumnVector Cluster::getAlphaUp()
{
    return this->alphaUp;
}
ColumnVector Cluster::getSigmaDown()
{
    return this->sigmaDown;
}

ColumnVector Cluster::getAlphaDown()
{
    return this->alphaDown;
}

void Cluster::setMu(ColumnVector mu)
{
    this->mu = mu;
}

void Cluster::setSigmaUp(ColumnVector sigma)
{
    this->sigmaUp = sigma;
}

void Cluster::setAlphaUp(ColumnVector alpha)
{
    this->alphaUp = alpha;
}
void Cluster::setSigmaDown(ColumnVector sigma)
{
    this->sigmaDown = sigma;
}

void Cluster::setAlphaDown(ColumnVector alpha)
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
