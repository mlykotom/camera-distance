#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QQueue>
#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QOpenGLTexture>
#include "distance_point.h"

#define FRAME_TIMEOUT_MS 10
#define RECT_SIZE 10

class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(QList<DistancePoint*> *_distancePointList, QQueue<QImage> *q_, QWidget *parent = 0);

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void mousePressEvent(QMouseEvent *e);

public slots:
    void onNewFrame();

signals:
    void measuringPointCoordsChanged(QPoint widgetPos, QSize widgetSize);

protected:
    void initializeGL();
    void paintEvent(QPaintEvent *event);
    void resizeGL(int width, int height);

private:
    QImage _image;
    GLuint background;
    QMutex _mutex;

    void setupViewPort(int width, int height);

    QList<DistancePoint*> *distancePointList;
    QQueue<QImage> *imageDistanceQueue;


    QOpenGLTexture* texture;
    void static drawFramePicture();
};

#endif // GLWIDGET_H
