#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <qfile.h>

#define WIDTH 320
#define HEIGHT 240
#define FPS 10

int centerX = WIDTH / 2;
int centerY = HEIGHT / 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(NULL)
{
    ui->setupUi(this);

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
}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    try{
        D3DFrame frame;
        Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
        frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
        frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
        frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
        frame.depth = cv::Mat3f(frameSize, CV_32FC3, pFrameData->depthData);

        int disparities = (camera->getParams().numDisparities * 16);

        Point p = Point(centerX, centerY);

        cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);

        cv::Rect testRect = cv::Rect_<int>(centerX - 10, centerY - 10, 20, 20);
        _leftRGB(testRect) = 0xFF0000;

        cv::Vec3f chro = depth.at<Vec3f>(p);
        float depthPoint = chro[2] / 10.0;

        float focal_length_pixels = 2 * 360;
        float baseline_mm = 30;
        float depthCalculated = (baseline_mm * focal_length_pixels / frame.disparity.at<float>(p)) / 100.0;

        ui->depth->setText(QString::number(depthCalculated) + " | " + QString::number(depthPoint));

        Mat disp8, rgbBDisparity;
        frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / disparities);
        cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);

        //farebna hlbkova mapa
//        cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);

        //zobrazenie
        ui->out_left->setImage(_leftRGB);
        ui->out_depth->setImage(rgbBDisparity);
    }
    catch(std::exception & error){
        qDebug() << error.what();
    }
}

MainWindow::~MainWindow()
{
    delete camera;
    delete ui;
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
