#include "image_output.h"
#include <QDebug>


ImageOutput::ImageOutput(QWidget *parent)
    :QWidget(parent)
{
    setMinimumSize(1, 1);
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::black);
}

void ImageOutput::mousePressEvent(QMouseEvent *e)
{
    qDebug() << e->pos();
}

void ImageOutput::setImage(const cv::Mat3b &image)
{
//    if(!_mutex.tryLock()) return;
//    _mutex.tryLock(5);
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
    painter.drawImage(event->rect(),_image);
//    _mutex.unlock();
}
