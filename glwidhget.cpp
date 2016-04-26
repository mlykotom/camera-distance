#include "glwidhget.h"
#include "QDebug"

GLWidhget::GLWidhget()
{
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);
}

QSize GLWidhget::minimumSizeHint() const
{
    return QSize(50, 50);

}

QSize GLWidhget::sizeHint() const
{
    return QSize(300, 200);

}

void GLWidhget::setImage(const cv::Mat3b &image, double distance)
{
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    update();


}

void GLWidhget::paintEvent(QPaintEvent *event)
{

    QMutexLocker lock(&_mutex);

    QPainter painter;
    painter.begin(this);
   // painter.drawRect(QRect(0,0,50,50));
    painter.drawImage(event->rect(),_image);
    painter.end();
}

