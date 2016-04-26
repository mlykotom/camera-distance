#include "image_output.h"
#include <QDebug>


ImageOutput::ImageOutput(QWidget *parent)
    :QWidget(parent),
      rectHeight(20),
      rectWidht(20)
{
    setMinimumSize(1, 1);
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);

    rect = QRectF(0,0,rectWidht,rectHeight);
    textPoint = QPointF(0,0);
    distanceString = "";
}

void ImageOutput::mousePressEvent(QMouseEvent *e)
{
    int x = e->pos().x()-10;
    int y = e->pos().y()-10;
    rect = QRectF(x,y,rectWidht,rectHeight);
    emit measuringPointCoordsChanged(x,y);
    //text ale to asi netreba
    //textPoint = QPointF(x,y+35);
}

void ImageOutput::setImage(const cv::Mat3b &image, double distance)
{
//    if(!_mutex.tryLock()) return;
//    _mutex.tryLock(5);
    QMutexLocker lock(&_mutex); // Lock to assure access
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    distanceString = QString::number(distance,'g',6);
    update();
//    _mutex.unlock();
}

void ImageOutput::paintEvent(QPaintEvent *event)
{
//    if(!_mutex.tryLock()) return;
    QMutexLocker lock(&_mutex);
    QPainter painter(this);
    painter.drawImage(event->rect(),_image);

    painter.setPen(Qt::red);
    painter.drawRect(rect);
    painter.end();
    //text
    //painter.drawText(textPoint,distanceString);
//    _mutex.unlock();
}
