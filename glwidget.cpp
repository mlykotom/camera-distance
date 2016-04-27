#include "glwidget.h"
#include "QDebug"
#include <QQueue>

#define FRAME_TIMEOUT_MS 10

GLWidget::GLWidget(bool showRect_, QList<QString> *distanceStringsList_, QQueue<int> *queue, QWidget *parent):
    QGLWidget(QGLFormat(QGL::DoubleBuffer),parent),
    rectHeight(20),
    rectWidht(20),
    showRect(showRect_),
    multipleMeasuringPoints(false),
    distanceStringsList(distanceStringsList_),
    myQueue(queue)
{
    _image = QImage(QSize(200,90), QImage::Format_RGB888);
    _image.fill(Qt::white);

    singleRect = QRectF(0,0,rectWidht,rectHeight);
    singleTextPoint = QPointF(0,0);
    singleDistanceString = "";


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

    if(multipleMeasuringPoints){
        rectList.append(QRectF(x,y,rectWidht,rectHeight));
        textPointsList.append(QPointF(x,y+35));
        //distanceStringsList.append("");
    }
    else{
        singleRect = QRectF(x,y,rectWidht,rectHeight);
        singleTextPoint = QPointF(x,y+35);
    }

    // TODO
    int frameX = (x / (double)this->width()) * 320;
    int frameY = (y / (double)this->height()) * 240;
    emit measuringPointCoordsChanged(frameX, frameY);
}

void GLWidget::setImage(const cv::Mat3b &image, double distance)
{
    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);

//    if(multipleMeasuringPoints)
//        distanceStringsList.last() = QString::number(distance,'f',2);
//    else
        singleDistanceString = QString::number(distance,'f',2);

    update();
}

void GLWidget::setImage(const cv::Mat3b &image)
{

    _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
   // distanceStringsList = distances;
//    qDebug()<<"tu este som";
//    qCopy(distances.begin(),distances.end(),distanceStringsList.begin());
//    qDebug()<<"tu uz ne";
    update();

}

void GLWidget::onNumberOfMeasuringPointsChanged(bool multipleMeasuringPoints_)
{
    multipleMeasuringPoints = multipleMeasuringPoints_;
}



void GLWidget::paintEvent(QPaintEvent *event)
{
    if(!_mutex.tryLock(FRAME_TIMEOUT_MS)) return;
    //    timer.start();
    QPainter painter;
    painter.begin(this);

    painter.drawImage(event->rect(),_image);

    if(showRect){
        painter.setPen(Qt::red);

        if(multipleMeasuringPoints){
            for(int i = 0; i < rectList.length() ; i++){
                painter.drawRect(rectList.at(i));
                painter.drawText(textPointsList.at(i),distanceStringsList->at(i));
            }
        }
        else{
            painter.drawRect(singleRect);
            painter.drawText(singleTextPoint,singleDistanceString);
        }
    }

    //    qDebug() << timer.elapsed();
    painter.end();

    if(myQueue->length() > 0){
        myQueue->dequeue();
    }
    _mutex.unlock();


}

