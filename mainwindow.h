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
    Ui::MainWindow *ui;
    StereoCamera *camera;
    cv::Mat _leftRGB, _rightRGB, _depthRGB;
    Mat colorLut;

   // std::queue leftQueue;
    QVector<cv::Mat> leftQueue;
    QVector<cv::Mat> rightQueue;
    QVector<cv::Mat> depthQueue;

    static void CALLBACK newFrameCallback(const PDense3DFrame pFrameData, void *pUserData){
        ((MainWindow *)pUserData)->onNewFrame(pFrameData);
    }

    void onNewFrame(const PDense3DFrame pFrameData);


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startProjecting();
};

#endif // MAINWINDOW_H
