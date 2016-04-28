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
    GLWidget(bool showRect_, QList<QString> *distanceStringsList_, QWidget *parent = 0);
    QTime timer;

    QSize minimumSizeHint() const;
    QSize sizeHint() const;

    void mousePressEvent(QMouseEvent *e);
    void setImage(const cv::Mat3b &image, double distance);
    void setImage(const cv::Mat3b &image);

public slots:
    void onNumberOfMeasuringPointsChanged(bool multipleMeasuringPoints_);
    void onPointsClear();

signals:
    void measuringPointCoordsChanged(int x, int y);

protected:

    void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;

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


};

#endif // GLWIDGET_H
