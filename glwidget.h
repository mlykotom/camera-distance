#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>
#include <QTime>

#include <QQueue>
#include <QGLWidget>
#include <QOpenGLFunctions>
#include <QGLShader>
#include <QOpenGLTexture>

QT_FORWARD_DECLARE_CLASS(QGLShaderProgram);


class GLWidget : public QGLWidget, protected QOpenGLFunctions
{
    Q_OBJECT

public:
    GLWidget(bool showRect_, QList<QString> *distanceStringsList_, QQueue<QImage> *q_, QWidget *parent = 0);
    QTime timer;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void mousePressEvent(QMouseEvent *e);
    void setImage(const cv::Mat3b &image, double distance);
    void setImage(const cv::Mat3b &image);

public slots:
    void onNumberOfMeasuringPointsChanged(bool multipleMeasuringPoints_);
    void onPointsClear();

    void onNewFrame();

signals:
    void measuringPointCoordsChanged(int x, int y);

protected:

    //void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
    void initializeGL();
    void paintGL();
    void resizeGL(int width, int height);

private:
    QImage _image;
    GLuint background;
    QMutex _mutex;

    int rectWidht;
    int rectHeight;

    QRectF singleRect;
    QPointF singleTextPoint;
    QString singleDistanceString;

    QList<QRectF> rectList;
    QList<QPointF> textPointsList;
    QList<QString> *distanceStringsList;

    bool showRect;
    bool multipleMeasuringPoints;

    QGLShaderProgram *program;
    QQueue<QImage> *q;
    QQueue<GLuint> textureQueue;


    GLuint tex;
    //QGLContext* mainContext;
    QOpenGLTexture* texture;
};

#endif // GLWIDGET_H
