#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QTime>
class GLWidget : public QGLWidget
{
    Q_OBJECT

public:
    GLWidget(bool showRect_,QWidget *parent = 0);
    QTime timer;

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
    QPointF textPoint;
    QString distanceString;

    bool showRect;
};

#endif // GLWIDGET_H
