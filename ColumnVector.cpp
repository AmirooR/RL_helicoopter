#include "ColumnVector.h"

float RowVector::operator*(ColumnVector& c)
{
    float sum = 0;
    for(std::size_t i=0; i < data.size(); i++)
        sum += c(i)*data[i];
    return sum;
}


