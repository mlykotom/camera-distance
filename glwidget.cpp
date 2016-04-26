#include "glwidget.h"
#include "QDebug"
#include <QQueue>

GLWidget::GLWidget(bool showRect_, QQueue<int> *queue, QWidget *parent):
    QOpenGLWidget(parent),
    rectHeight(20),
    rectWidht(20),
    showRect(showRect_),
    myQueue(queue)
{
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);

    rect = QRectF(0,0,rectWidht,rectHeight);
    textPoint = QPointF(0,0);
    distanceString = "";
}

QSize GLWidget::minimumSizeHint() const
{
    return QSize(50, 50);

}

QSize GLWidget::sizeHint() const
{
    return QSize(300, 200);

}

void GLWidget::mousePressEvent(QMouseEvent *e)
{
    if(!showRect) return;

    int x = e->pos().x()-10;
    int y = e->pos().y()-10;
    rect = QRectF(x,y,rectWidht,rectHeight);
    textPoint = QPointF(x,y+35);
    // TODO
    int frameX = (x / (double)this->width()) * 320;
    int frameY = (y / (double)this->height()) * 240;
    emit measuringPointCoordsChanged(frameX, frameY);
}

void GLWidget::setImage(const cv::Mat3b &image, double distance)
{
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
    distanceString = QString::number(distance,'f',2);
    update();
}

#define FRAME_TIMEOUT_MS 10

void GLWidget::paintEvent(QPaintEvent *event)
{
    if(!_mutex.tryLock(FRAME_TIMEOUT_MS)) return;
    //    timer.start();
    QPainter painter;
    painter.begin(this);

    painter.drawImage(event->rect(),_image);

    if(showRect){
        painter.setPen(Qt::red);
        painter.drawRect(rect);
        //text
        painter.drawText(textPoint,distanceString);
    }

    //    qDebug() << timer.elapsed();
    painter.end();

    if(myQueue->length() > 0){
        myQueue->dequeue();
    }
    _mutex.unlock();
}
