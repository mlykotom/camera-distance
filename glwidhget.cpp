#include "glwidhget.h"
#include "QDebug"

GLWidhget::GLWidhget(bool showRect_, QWidget *parent):
    QGLWidget(QGLFormat(QGL::DoubleBuffer), parent),
    rectHeight(20),
    rectWidht(20),
    showRect(showRect_)
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
    if(!showRect) return;

    int x = e->pos().x()-10;
    int y = e->pos().y()-10;
    rect = QRectF(x,y,rectWidht,rectHeight);

    // TODO
    int frameX = (x / (double)this->width()) * 320;
    int frameY = (y / (double)this->height()) * 240;
    emit measuringPointCoordsChanged(frameX, frameY);

}

void GLWidhget::setImage(const cv::Mat3b &image, double distance)
{
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    update();


}

void GLWidhget::paintEvent(QPaintEvent *event)
{
    QPainter painter;
    painter.begin(this);
    painter.drawImage(event->rect(),_image);

    if(showRect){
        painter.setPen(Qt::red);
        painter.drawRect(rect);
    }


    painter.end();
}

