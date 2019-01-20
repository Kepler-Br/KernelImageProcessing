#include "convmatfactory.h"

#include <thread>
#include <chrono>
#include <queue>
#include <cstdlib>
#include <cmath>

using namespace MatAlg;
// Formula:
//https://www.geeksforgeeks.org/gaussian-filter-generation-c/
ConvMatFactory::ConvMatFactory()
{

}

double ConvMatFactory::gaussKernelFormula(const double x, const double y)
{
    double sigma = 1.0;
    double firstPart = 1/(2*M_PI*pow(sigma, 2));
    double secondPart = exp(-(pow(x, 2) + pow(y, 2))/(2*pow(sigma, 2)));
    return firstPart*secondPart;
}

Matrix<double> ConvMatFactory::gaussBlurMatrix(const uint column, const uint row)
{
    Matrix<double> mat(row, column, 0.0);
    int centerX = row/2;
    int centerY = column/2;
    for(int x = 0; x < int(row); x++)
    {
        for(int y = 0; y < int(column); y++)
        {
            double value = gaussKernelFormula(x-centerX, y-centerY);
            mat[x][y] = value;
        }
    }
    return mat;
}

MatAlg::Matrix<double> ConvMatFactory::blurMatrix(const uint column, const uint row)
{
    return Matrix<double>(row, column, 1.0);
}

Matrix<double> ConvMatFactory::edgeDetectMatrix()
{
    return {{0, 1, 0},
            {1, -4, 1},
            {0, 1, 0}};
}

Matrix<double> ConvMatFactory::edgeEnhanceMatrix()
{
    return {{0, 0, 0},
            {-1, 1, 0},
            {0, 0, 0}};
}

MatAlg::Matrix<double> ConvMatFactory::sharpenMatrix()
{
    return {{0, -1, 0},
            {-1, 5, -1},
            {0, -1, 0}};
}

MatAlg::Matrix<double> ConvMatFactory::embossMatrix()
{
    return {{-2, -1, 0},
            {-1, 1, 1},
            {0, 1, 2}};
}
