#ifndef GLWIDHGET_H
#define GLWIDHGET_H

#include <QGLWidget>
#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>

class GLWidhget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidhget(bool showRect_,QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void mousePressEvent(QMouseEvent *e);


    void setImage(const cv::Mat3b &image, double distance);

signals:
    void measuringPointCoordsChanged(int x, int y);


protected:

    void paintEvent(QPaintEvent *event);


private:
    QImage _image;
    GLuint background;
    QMutex _mutex;

    QRectF rect;
    int rectWidht;
    int rectHeight;

    bool showRect;


};

#endif // GLWIDHGET_H
