#ifndef IMAGEWORKER_H
#define IMAGEWORKER_H

#include <QThread>
#include <QDebug>
#include <QImage>
#include "matrix.h"
#include "imageprocessor.h"

class ImageWorker: public QThread
{
private:
    MatAlg::Matrix<double> *filter;
    QImage img;
    QImage processed;
    double offset;
    double divisor;
    int threadsNum;
    int passes;
    void run()
    {
        processed = img;
        for(int i = 0; i < passes; i++)
            processed = ImageProcessor::run(*filter, processed, offset, divisor, uint(threadsNum));
    }
public:
    ImageWorker();

    void setup(MatAlg::Matrix<double> &filter, const double &offset, const double &divisor, QImage &img, const int threadsNum, const int passes)
    {
        this->filter = &filter;
        this->img = img;
        this->offset = offset;
        this->divisor = divisor;
        this->threadsNum = threadsNum;
        this->passes = passes;
    }

    QImage getProcessedImage()
    {
        return processed;
    }
};

#endif // IMAGEWORKER_H
