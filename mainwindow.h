#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QPaintEngine>
#include <QPaintEvent>
#include <opencv2/opencv.hpp>
#include "DUOLib.h"
#include "Dense3DMT.h"
#include "api_keys.h"

using namespace cv;

namespace Ui {
class MainWindow;
}

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
    void setImage(const Mat3b &image)
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

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);

private:
    Ui::MainWindow *ui;

    static void CALLBACK newFrameCb(const PDense3DFrame pFrameData, void *pUserData){
        ((MainWindow *)pUserData)->onNewFrame(pFrameData);
    }

    void onNewFrame(const PDense3DFrame pFrameData);

private:
    DUOInstance _duo;
    Dense3DMTInstance _dense;
    ImageOutput *_img[3];
    cv::Mat _leftRGB, _rightRGB, _disparityRGB;
};

#endif // MAINWINDOW_H
