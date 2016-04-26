#ifndef GLWIDHGET_H
#define GLWIDHGET_H

#include <QGLWidget>
#include <opencv2/opencv.hpp>
#include <QPaintEngine>
#include <QPaintEvent>

class GLWidhget : public QGLWidget
{
public:
    GLWidhget();

    QSize minimumSizeHint() const;
    QSize sizeHint() const;
    void setImage(const cv::Mat3b &image, double distance);

protected:

    void paintEvent(QPaintEvent *event);


private:
    QImage _image;
    GLuint background;
    QMutex _mutex;

};

#endif // GLWIDHGET_H
