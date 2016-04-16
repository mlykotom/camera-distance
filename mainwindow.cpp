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

        cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);

        Mat disp8, rgbBDisparity;
        frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / (camera->getParams().numDisparities * 16));
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
