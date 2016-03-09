#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QtWidgets>

#include <queue>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "DUOLib.h"
#include "Dense3DMT.h"
#include "api_keys.h"
#include "image_output.h"
#include "stereocamera.h"

using namespace cv;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

private:
    StereoCamera *camera;
    ImageOutput *_img[3];
    cv::Mat _leftRGB, _rightRGB, _depthRGB;
    QLabel *test;
    Ui::MainWindow *ui;
    Mat colorLut;

private:
    static void CALLBACK newFrameCallback(const PDense3DFrame pFrameData, void *pUserData){
        ((MainWindow *)pUserData)->onNewFrame(pFrameData);
    }

    void onNewFrame(const PDense3DFrame pFrameData);

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    void closeEvent(QCloseEvent *);
};

#endif // MAINWINDOW_H
