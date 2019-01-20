#ifndef IMAGEPROCESSER_H
#define IMAGEPROCESSER_H

#include <QImage>

#include <queue>
#include <mutex>

#include "matrix.h"

class ImageProcessor
{
private:
    ImageProcessor();

    enum ColorChannel {
        RED = 0, GREEN, BLUE
    };

    static MatAlg::Matrix<double> getImageKernel(const QImage &img, const uint centerX,
                                                     const uint centerY, const uint rows, const uint columns,
                                                     const ColorChannel channel);

    /*!
       \brief process every pixel in this thread function.
    */
    static void threadFunction(const QImage *img, QImage *result,
                                 const MatAlg::Matrix<double> *filter, const double offset,
                                 const double divisor, std::queue<std::tuple<uint, uint> > *tasks,
                                 std::mutex *mut);

    /*!
       \brief function for clamping color between 255 and 0.
    */
    static double clampColor(const double &color);
public:
    static QImage run(MatAlg::Matrix<double> filter, const QImage &img,
                             const double offset, double divisor, const uint threads);

};

#endif // IMAGEPROCESSER_H
