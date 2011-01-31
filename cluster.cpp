#include "cluster.h"
#include "util.h"

using namespace std;

Cluster::Cluster()
{
}

Cluster::Cluster(ColumnVector mu, ColumnVector sigma, ColumnVector alpha)
{
    if (mu.dim1() == sigma.dim1() && alpha.dim1() == mu.dim1())
    {
        this->mu = mu;
        this->sigma = sigma;
        this->alpha = alpha;
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

ColumnVector Cluster::getSigma()
{
    return this->sigma;
}

ColumnVector Cluster::getAlpha()
{
    return this->alpha;
}

void Cluster::setMu(ColumnVector mu)
{
    this->mu = mu;
}

void Cluster::setSigma(ColumnVector sigma)
{
    this->sigma = sigma;
}

void Cluster::setAlpha(ColumnVector alpha)
{
    this->alpha = alpha;
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
    for (int i = 0; i < tr.dim1(); i++)
    {
        if (dist(i) > tr(i))
        {
            return false;
        }
    }
    return true;
}
