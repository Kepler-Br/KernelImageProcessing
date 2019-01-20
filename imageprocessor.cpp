#include "imageprocessor.h"

#include <thread>

#include "matrixmisc.h"
#include "matrix.h"
#include <iostream>

using namespace MatAlg;

ImageProcessor::ImageProcessor()
{

}

Matrix<double> ImageProcessor::getImageKernel(const QImage &img, const uint centerX,
                                              const uint centerY, const uint rows, const uint columns,
                                              const ColorChannel channel)
{
    Matrix<double> mat(rows, columns, 0.0);
    for(uint x = 0; x < rows; x++)
    {
        for(uint y = 0; y < columns; y++)
        {
            QColor col = QColor(img.pixel(centerX+x-rows/2-1, centerY+y-columns/2-1));

            if(channel == ColorChannel::RED)
                mat[x][y] = double(col.red());
            if(channel == ColorChannel::GREEN)
                mat[x][y] = double(col.green());
            if(channel == ColorChannel::BLUE)
                mat[x][y] = double(col.blue());
        }
    }
    return mat;
}

QImage ImageProcessor::run(MatAlg::Matrix<double> filter, const QImage &img,
                          const double offset, double divisor, const uint threadNum)
{

    if(filter.getColumnSize() == filter.getRowSize())
        filter = transpose(filter);

    uint width = img.width();
    uint height = img.height();
    uint columns = filter.getColumnSize();
    uint rows = filter.getRowSize();
    QImage result(width, height, QImage::Format::Format_RGB32);
    result.fill(0xFFFFFF);
    if(divisor == 0.0)
        divisor = elementSum(filter);

    std::queue<std::tuple<uint, uint>> tasks;
    std::vector<std::thread> threads;
    std::mutex mut;

//    std::cout << "Creating tasks.\n";
    for(uint x = rows; x < width-rows; x++)
    {
        for(uint y = columns; y < height-columns; y++)
        {
            tasks.push(std::make_tuple(x, y));
        }
    }

//    std::cout << "Creating threads.\n";
    for(uint i = 0; i < threadNum; i++)
    {
        threads.push_back(std::thread(threadFunction, &img, &result, &filter, offset, divisor, &tasks, &mut));
    }

//    std::cout << "Waiting for threads.\n";
    for(uint i = 0; i < threadNum; i++)
    {
        threads[i].join();
    }

    return result;
}

void ImageProcessor::threadFunction(const QImage *img, QImage *result,
                                    const Matrix<double> *filter,
                                    const double offset, const double divisor,
                                    std::queue<std::tuple<uint, uint> > *tasks, std::mutex *mut)
{
    while(true)
    {
        // Get task from queue.
        mut->lock();
        if(tasks->empty())
        {
            mut->unlock();
            return;
        }

        uint x, y;
        std::tie(x, y) = tasks->front();
        tasks->pop();
        mut->unlock();

        uint columns = filter->getColumnSize();
        uint rows = filter->getRowSize();

        // Process image.
        // Get image color channels loaded into matrix for futher process.
        Matrix<double> redChannel = getImageKernel(*img, x, y, rows, columns, ColorChannel::RED);
        double redResult = elementMul(*filter, redChannel);
        redResult = redResult/divisor+offset;

        Matrix<double> blueChannel = getImageKernel(*img, x, y, rows, columns, ColorChannel::BLUE);
        double blueResult = elementMul(*filter, blueChannel);
        blueResult = blueResult/divisor+offset;

        Matrix<double> greenChannel = getImageKernel(*img, x, y, rows, columns, ColorChannel::GREEN);
        double greenResult = elementMul(*filter, greenChannel);
        greenResult = greenResult/divisor+offset;

        // Clamp result, so u_char will not overflow.
        redResult = clampColor(redResult);
        greenResult = clampColor(greenResult);
        blueResult = clampColor(blueResult);

        QColor color;
        color.setRed(redResult);
        color.setGreen(greenResult);
        color.setBlue(blueResult);
//        vec3b color;
//        color.r = u_char(redResult);
//        color.g = u_char(greenResult);
//        color.b = u_char(blueResult);
        result->setPixel(x, y, color.rgb());
    }
}

double ImageProcessor::clampColor(const double &color)
{
    if(color >= 0 && color <= 255)
        return color;
    if(color < 0)
        return 0;
    if(color > 255)
        return 255;
    // -Wreturn-type
    return color;
}
