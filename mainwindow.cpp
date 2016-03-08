#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

#define WIDTH 320
#define HEIGHT 240
#define FPS 10

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

    colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    try{
        camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);
        camera->setParams();
        camera->start(newFrameCallback, this);
    }
    catch(std::invalid_argument &error){
        qDebug() << error.what();
    }
}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    D3DFrame frame;
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
    frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
    frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);

    cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);
    Q_EMIT _img[0]->setImage(_leftRGB);

    cvtColor(frame.rightImg, _rightRGB, COLOR_GRAY2BGR);
    Q_EMIT _img[1]->setImage(_rightRGB);

    Mat disp8;
    frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / (camera->getParams().numDisparities * 16));
    Mat rgbBDisparity;
    cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
    cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);

    Q_EMIT _img[2]->setImage(rgbBDisparity);
}

MainWindow::~MainWindow()
{
//    delete ui;
    delete camera;
}

void MainWindow::closeEvent(QCloseEvent *event){

}
