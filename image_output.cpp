#include "image_output.h"
#include <QDebug>


ImageOutput::ImageOutput(QWidget *parent)
    :QWidget(parent)
{
    setMinimumSize(1, 1);
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::black);
}

void ImageOutput::setImage(const cv::Mat3b &image)
{
//    if(!_mutex.tryLock()) return;
    QMutexLocker lock(&_mutex); // Lock to assure access
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    update();
//    _mutex.unlock();
}

void ImageOutput::paintEvent(QPaintEvent *event)
{
//    if(!_mutex.tryLock()) return;
    QMutexLocker lock(&_mutex);
    QPainter painter(this);
//    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
//    painter.drawPixmap(event->rect(), QPixmap::fromImage(_image));
    painter.drawImage(event->rect(),_image);
//    _mutex.unlock();
}
