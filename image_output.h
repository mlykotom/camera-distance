#ifndef IMAGE_OUTPUT_H
#define IMAGE_OUTPUT_H

#include <QWidget>
#include <QPaintEngine>
#include <QPaintEvent>
#include <opencv2/opencv.hpp>

class ImageOutput : public QWidget
{
    Q_OBJECT
public:
    ImageOutput(QWidget* parent = 0);
    void mousePressEvent(QMouseEvent *e);
public slots:
    // Mat imag must be BGR image
    void setImage(const cv::Mat3b &image, double distance);
private:
    void paintEvent(QPaintEvent *event);
private:
    QImage _image;
    QMutex _mutex;
    QRectF rect;
    QPointF textPoint;
    QString distanceString;
    int rectWidht;
    int rectHeight;

signals:
    void measuringPointCoordsChanged(int x, int y);
};

#endif // IMAGE_OUTPUT_H
