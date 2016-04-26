#include "glwidhget.h"
#include "QDebug"

GLWidhget::GLWidhget(QWidget *parent):
    QGLWidget(parent),
    rectHeight(20),
    rectWidht(20)
{
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);

    rect = QRectF(0,0,rectWidht,rectHeight);
}

QSize GLWidhget::minimumSizeHint() const
{
    return QSize(50, 50);

}

QSize GLWidhget::sizeHint() const
{
    return QSize(300, 200);

}

void GLWidhget::mousePressEvent(QMouseEvent *e)
{
    int x = e->pos().x()-10;
    int y = e->pos().y()-10;
    rect = QRectF(x,y,rectWidht,rectHeight);
    emit measuringPointCoordsChanged(x,y);
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

    painter.setPen(Qt::red);
    painter.drawRect(rect);


    painter.end();
}

