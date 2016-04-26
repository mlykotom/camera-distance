#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QOpenGLWidget>
#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QTime>
class GLWidget : public QOpenGLWidget
{
    Q_OBJECT

public:
    GLWidget(bool showRect_,QQueue<int> *queue, QWidget *parent = 0);
    QTime timer;

    QQueue<int> *myQueue;
    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void mousePressEvent(QMouseEvent *e);
    void setImage(const cv::Mat3b &image, double distance);

signals:
    void measuringPointCoordsChanged(int x, int y);

protected:

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

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
