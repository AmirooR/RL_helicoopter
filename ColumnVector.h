#ifndef __AVECTOR_H_
#define __AVECTOR_H_

#include <vector>
#include <iostream>

using namespace std;

class AVector
{
    protected:
    int type;
    vector<float> data;
    public:
        AVector(unsigned int x)
        {
            type = 0;
            data.resize(x);
        }

        float& operator()(unsigned int idx)
        {
            return data[idx];
        }

        void resize(unsigned int x)
        {
            data.resize(x);
        }

        friend AVector operator+(AVector a, AVector& rhs)
        {
            for(unsigned int i=0;i<a.dim1();i++)
                a(i) = a(i) + rhs(i);
            return a;
        }

        AVector operator-(AVector& rhs)
        {
            AVector a(data.size());
            for(unsigned int i=0;i<data.size();i++)
                a(i) = data[i] - rhs(i);
            return a;
        }

        AVector& operator=(AVector rhs)
        {
            this->resize(rhs.dim1() );
            for(unsigned int i=0;i<data.size();i++)
                data[i] = rhs(i);
            return *this;
        }

        AVector operator*(AVector& rhs)
        {
            AVector a(data.size());
            for(std::size_t i=0;i<data.size();i++)
                a(i) = data[i] * rhs(i);
            return a;
        }

        AVector operator*(const float rhs)
        {
            AVector a(data.size());
            for(std::size_t i=0;i<data.size();i++)
                a(i) = data[i] * rhs;
            return a;
        }

        AVector operator/(const float rhs)
        {
            AVector a(data.size());
            for(std::size_t i=0;i<data.size();i++)
                a(i) = data[i] / rhs;
            return a;
        }

        unsigned int dim1()
        {
            return data.size();
        }

        void print()
        {
            for(int i=0;i<data.size();i++)
                cout<<data[i]<<endl;
        }

};

class ColumnVector;

class RowVector:public AVector
{
    public:
        RowVector(unsigned int x):AVector(x)
        {
            type = 2;
        }
        float operator*(ColumnVector& c);
};

class ColumnVector:public AVector
{
    public:
        ColumnVector():AVector(1)
        {
            type = 1;
        }
        ColumnVector(unsigned int x):AVector(x)
        {
            type = 1;
        }

        ColumnVector operator-(ColumnVector& rhs)
        {
            ColumnVector a(data.size());
            for(unsigned int i=0;i<data.size();i++)
                a(i) = data[i] - rhs(i);
            return a;
        }

        ColumnVector operator+(ColumnVector& rhs)
        {
            ColumnVector a(data.size());
            for(unsigned int i=0;i<data.size();i++)
                a(i) = data[i] + rhs(i);
            return a;
        }

        ColumnVector operator*(const float rhs)
        {
            ColumnVector a(data.size());
            for(std::size_t i=0;i<data.size();i++)
                a(i) = data[i] * rhs;
            return a;
        }

        ColumnVector operator/(const float rhs)
        {
            ColumnVector a(data.size());
            for(std::size_t i=0;i<data.size();i++)
                a(i) = data[i] / rhs;
            return a;
        }

        RowVector transpose()
        {
            RowVector r(data.size());
            for(std::size_t i=0;i<data.size();i++)
                r(i) = data[i];
            return r;
        }
};

#endif
