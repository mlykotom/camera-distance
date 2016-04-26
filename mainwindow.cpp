#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <qfile.h>

#define WIDTH 320
#define HEIGHT 240
#define FPS 20

int centerX = WIDTH / 2;
int centerY = HEIGHT / 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(NULL)
{
    ui->setupUi(this);

    glDistanceWidget = new GLWidhget(true,this);
    ui->glDistanceLayout->addWidget(glDistanceWidget,0,0);

    glDepthWidget = new GLWidhget(false,this);
    ui->glDepthLayout->addWidget(glDepthWidget,0,0);

    colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    try{
        camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);
        camera->open();
        camera->setParams();

        camera->start(newFrameCallback, this);
    } catch(std::invalid_argument * error){
        QMessageBox::warning(this, "Invalid argument", error->what());
    }

    //    measuringPoint.setX(centerX);
    //    measuringPoint.setY(centerY);
    p = Point(centerX, centerY);

    //connect(ui->out_left,SIGNAL(measuringPointCoordsChanged(int,int)),this,SLOT(onMeasuringPointCoordsChanged(int,int)));
    connect(glDistanceWidget,SIGNAL(measuringPointCoordsChanged(int,int)),this,SLOT(onMeasuringPointCoordsChanged(int,int)));



}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){

    if(!_mutex.tryLock(50)) return;

    //  QMutexLocker lock(&_mutex);

    try{
        D3DFrame frame;
        Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
        frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
        frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
        frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
        frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);


        if(ui->tabWidget->currentIndex() == 0){
            cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);
            //render
            glDistanceWidget->setImage(_leftRGB,0);

            // distance from depth map
            cv::Vec3f chro = frame.depth.at<Vec3f>(p);
            float depthPoint = chro[2] / 10.0;

            //calculated distance
            //        float focal_length_pixels = 2 * 360;
            //        float baseline_mm = 30;
            //        float depthCalculated = (baseline_mm * focal_length_pixels /
            //                                 frame.disparity.at<float>(p)) / 100.0;

            //show distance
            ui->depth->setText(QString::number(depthPoint));
        }

        if(ui->tabWidget->currentIndex() == 1){
            int disparities = (camera->getParams().numDisparities * 16);
            Mat disp8, rgbBDisparity;
            frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / disparities);
            cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
            //farebna hlbkova mapa
            cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);
            //render
           glDepthWidget->setImage(rgbBDisparity,0);

        }

        _mutex.unlock();
    }
    catch(std::exception & error){
        qDebug() <<"error:" << error.what();
    }
}

MainWindow::~MainWindow()
{
    //tu to pada pri zavreni okna
    //    if (camera != NULL)
    //        delete camera;
    delete ui;
}

void MainWindow::onMeasuringPointCoordsChanged(int x, int y)
{
    //    measuringPoint.setX(x);
    //    measuringPoint.setY(y);
    p = Point(x, y);
}

void MainWindow::on_ledSlider_valueChanged(int value)
{
    camera->setLed(value* 0.1);
    qDebug() << camera->getLed();
}

void MainWindow::on_gainSlider_valueChanged(int value)
{
    camera->setGain(value* 0.1);
    qDebug() << camera->getGain();
}

void MainWindow::on_exposureSlider_valueChanged(int value)
{
    camera->setExposure(value* 0.1);
    qDebug() << camera->getExposure();
}

void MainWindow::on_swapVerticalCheckbox_clicked(bool checked)
{
    camera->setVerticalFlip(checked);
    qDebug() << camera->getVerticalFlip();
}
