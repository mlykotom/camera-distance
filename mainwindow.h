#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QDesktopWidget>
#include <QtWidgets>
#include <QQueue>
#include <QSharedPointer>

#include <stdexcept>
#include <opencv2/opencv.hpp>
#include "DUOLib.h"
#include "api_keys.h"
#include "stereocamera.h"
#include "glwidget.h"
#include "distance_point.h"
#include "thread_safe_queue.h"

#define CAMERA_WIDTH 320
#define CAMERA_HEIGHT 240
#define CAMERA_FPS 30
#define CAMERA_BASELINE_MM 30
#define CAMERA_FOCAL_LENGTH_MM 2.05

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
    cv::Mat colorLut;

    float focal_length;
    float baseline_mm;
    int disparities;

    GLWidget *glDistanceWidget;
    GLWidget *glDepthWidget;
    QMutex _mutex;
    QThread *frameThread;

    QList<QSharedPointer<DistancePoint>> *renderingPoints;
    ThreadSafeQueue<QImage> *distanceQueue;
    ThreadSafeQueue<QImage> *depthQueue;

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
    void onMeasuringPointCoordsChanged(QPoint pos, QSize widgetSize);
    void setUpCamera();

signals:
    void newDistanceFrame();
    void newDepthFrame();

private slots:
    void on_ledSlider_valueChanged(int value);
    void on_gainSlider_valueChanged(int value);
    void on_exposureSlider_valueChanged(int value);
    void on_swapVerticalCheckbox_clicked(bool checked);
    void on_clearPoints_clicked();
    void showAuthorsDialog();
    void on_connectCameraButton_clicked();
};

#endif // MAINWINDOW_H
