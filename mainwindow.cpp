#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <qfile.h>

#define WIDTH 320
#define HEIGHT 240
#define FPS 10

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    camera(NULL)
{

    // TODO handle UI with form
    resize(WIDTH*3, HEIGHT);
    setWindowTitle("DUO Test");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QSplitter *hs = new QSplitter();
    hs->addWidget(_img[0] = new ImageOutput());
    hs->addWidget(_img[1] = new ImageOutput());
    hs->addWidget(_img[2] = new ImageOutput());
    test = new QLabel();

    hs->addWidget(test);
    setCentralWidget(hs);

    colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    try{
        camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);
        camera->open();
        camera->setParams();
        camera->start(newFrameCallback, this);
    }
    catch(std::invalid_argument *error){
        qDebug() << error->what();
    }
}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    try{
    D3DFrame frame;
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
    frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
    frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);

    float depth1 = frame.depth.at<float>(0, 0);
    float depth2 = frame.depth.at<float>(WIDTH / 2, HEIGHT / 2);
//    float depth3 = frame.depth.at<float>(WIDTH -1, HEIGHT - 1);

    test->setText(QString::number(depth1) + "|" + QString::number(depth2));

    cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);
    _img[0]->setImage(_leftRGB);

    cvtColor(frame.rightImg, _rightRGB, COLOR_GRAY2BGR);
    _img[1]->setImage(_rightRGB);

    Mat disp8, rgbBDisparity;
    frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / (camera->getParams().numDisparities * 16));
    cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
//    cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);
    _img[2]->setImage(rgbBDisparity);
    }
    catch(std::exception & error){
        qDebug() << error.what();
    }
}

MainWindow::~MainWindow()
{
//    delete ui;
    delete camera;
}

void MainWindow::closeEvent(QCloseEvent *event){

}
