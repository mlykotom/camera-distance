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
    ImageOutput()
    {
        setMinimumSize(1, 1);
        _image = QImage(QSize(1,1), QImage::Format_RGB888);
        _image.fill(Qt::black);
    }
public Q_SLOTS:
    // Mat imag must be BGR image
    void setImage(const cv::Mat3b &image)
    {
        QMutexLocker lock(&_mutex); // Lock to assure access
        _image = QImage(image.data, image.cols, image.rows, QImage::Format_RGB888);
        update();
    }
private:
    void paintEvent(QPaintEvent *event)
    {
        QMutexLocker lock(&_mutex);
        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);
        painter.drawPixmap(event->rect(), QPixmap::fromImage(_image));
    }
private:
    QImage _image;
    QMutex _mutex;
};

#endif // IMAGE_OUTPUT_H
