#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <stdexcept>

#define WIDTH 320
#define HEIGHT 240
#define FPS 30

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    camera(NULL)
{
    // TODO handle UI with form
    resize(320*2, 240);
    setWindowTitle("DUO Test");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QSplitter *hs = new QSplitter();
    hs->addWidget(_img[0] = new ImageOutput());
    hs->addWidget(_img[1] = new ImageOutput());
    hs->addWidget(_img[2] = new ImageOutput());
    setCentralWidget(hs);

    this->colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    try{
        camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);
        camera->setParams();
        camera->start(newFrameCallback, this);
    }
    catch(std::runtime_error &error){
        qDebug() << error.what();
    }
}


void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    Frame3D frame;
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
    frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
    frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);

//  qDebug() << pFrameData->disparityData[0];
//  qDebug() << pFrameData->disparityData[1];
//  qDebug() << "----";
//    qDebug() << "[" << pFrameData->depthData->x << "," << pFrameData->depthData->y << "," << pFrameData->depthData->z << "]";

    cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);
    Q_EMIT _img[0]->setImage(_leftRGB);

    cvtColor(frame.rightImg, _rightRGB, COLOR_GRAY2BGR);
    Q_EMIT _img[1]->setImage(_rightRGB);

//    GetDense3Params(dense3) //

//    Mat disp8;
//    frame.disparity.convertTo(disp8, CV_8UC1, 255.0/20*16); //params.numDisparities instead of 2
//    Mat rgbDepth;
//    cv::cvtColor(disp8, rgbDepth, COLOR_GRAY2BGR);
//    cv::LUT(rgbDepth, colorLut, rgbDepth);

//    Q_EMIT _img[2]->setImage(rgbDepth);
}

MainWindow::~MainWindow()
{
//    delete ui;
    delete camera;
}

void MainWindow::closeEvent(QCloseEvent *event){

}
