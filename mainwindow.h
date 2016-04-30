#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QQueue>

#include <queue>
#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "DUOLib.h"
#include "api_keys.h"
#include "stereocamera.h"
#include "glwidget.h"

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
    cv::Mat _leftRGB, _depthRGB;
    Mat colorLut;

    float focal_length_pixels;
    float baseline_mm;
    int disparities;

    GLWidget *glDistanceWidget;
    GLWidget *glDepthWidget;
    QMutex _mutex;
    QLabel *distanceLabel;

    Point p;
    QList<Point> measuringPointsList;
    QList<QString> *distancesList;

    QQueue<QPair<QImage,float>> *distanceQueue;
    QQueue<QPair<QImage,float>> *depthQueue;

    static void CALLBACK newFrameCallback(const PDense3DFrame pFrameData, void *pUserData){
        ((MainWindow *)pUserData)->onNewFrame(pFrameData);
    }

    void onNewFrame(const PDense3DFrame pFrameData);
    void distanceCalculation(const PDense3DFrame pFrameData);
    void depthCalculation(const PDense3DFrame pFrameData);

    void createMenu();

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void startProjecting();

public slots:
    void onMeasuringPointCoordsChanged(int x, int y);
    void setUpCamera();

signals:
    void newDistanceFrame();
    void newDepthFrame();

private slots:
    void on_ledSlider_valueChanged(int value);
    void on_gainSlider_valueChanged(int value);
    void on_exposureSlider_valueChanged(int value);
    void on_swapVerticalCheckbox_clicked(bool checked);
    void showAuthorsDialog();
};

#endif // MAINWINDOW_H
