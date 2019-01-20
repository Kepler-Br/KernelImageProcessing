#ifndef CONVMATFACTORY_H
#define CONVMATFACTORY_H

#include "matrix.h"

class ConvMatFactory
{
private:
    ConvMatFactory();

    // Formula:
    //https://www.geeksforgeeks.org/gaussian-filter-generation-c/
    /*!
       \brief function that calculates matrix element for gaussian blur.
    */
    static double gaussKernelFormula(const double x, const double y);

public:
    static MatAlg::Matrix<double> gaussBlurMatrix(const uint column, const uint row);
    static MatAlg::Matrix<double> blurMatrix(const uint column, const uint row);
    static MatAlg::Matrix<double> edgeDetectMatrix();
    static MatAlg::Matrix<double> edgeEnhanceMatrix();
    static MatAlg::Matrix<double> sharpenMatrix();
    static MatAlg::Matrix<double> embossMatrix();
};

#endif // CONVMATFACTORY_H
