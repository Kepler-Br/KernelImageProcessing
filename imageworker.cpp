#include "imageworker.h"

ImageWorker::ImageWorker():
    img(1, 1, QImage::Format::Format_RGB32),
    processed(1, 1, QImage::Format::Format_RGB32)
{

}
