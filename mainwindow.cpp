#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"
#include <qfile.h>

#define WIDTH 320
#define HEIGHT 240
#define FPS 10

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(NULL)
{

    ui->setupUi(this);

    colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);



    camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);

    if(camera->open()){

        camera->setParams();
        camera->start(newFrameCallback, this);
    }
    else
        qDebug()<<"Camera not opened!";

    leftQueue.resize(0);
    rightQueue.resize(0);
    depthQueue.resize(0);

}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    try{
        D3DFrame frame;
        Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
        frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
        frame.rightImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->rightData);
        frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
        frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);


        //  float depth1 = frame.depth.at<float>(0, 0);
          float depth2 = frame.depth.at<float>(WIDTH / 2, HEIGHT / 2);
        //  float depth3 = frame.depth.at<float>(WIDTH -1, HEIGHT - 1);

        cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);
        cvtColor(frame.rightImg, _rightRGB, COLOR_GRAY2BGR);

        Mat disp8, rgbBDisparity;
        frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / (camera->getParams().numDisparities * 16));
        cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
//        qDebug()<< "x: " << pFrameData->depthData->x << "y: "
//                <<pFrameData->depthData->y << "z: "<< pFrameData->depthData->z
//               <<"depthMid:" << depth2;

        //farebna hlbkova mapa
        //cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);

      //zobrazenie
        ui->out_left->setImage(_leftRGB);
        ui->out_right->setImage(_rightRGB);
        ui->out_depth->setImage(rgbBDisparity);

//        if(leftQueue.count()  < 1000) leftQueue.push_back(_leftRGB);
//        if(rightQueue.count() < 1000) rightQueue.push_back(_rightRGB);
//        if(depthQueue.count() < 1000) depthQueue.push_back(rgbBDisparity);
    }
    catch(std::exception & error){
        qDebug() << error.what();
    }
}

void MainWindow::startProjecting()
{
    while(cvWaitKey(cvWaitKey(1) & 0xff) != 27){
  //  while(1){
        if(!leftQueue.empty()){

            ui->out_left->setImage(leftQueue.front());
            leftQueue.pop_front();
        }
        if(!rightQueue.empty()){

            ui->out_right->setImage(rightQueue.front());
            rightQueue.pop_front();
        }
        if(!depthQueue.empty()){

            ui->out_depth->setImage(depthQueue.front());
            depthQueue.pop_front();
        }

   }
}

MainWindow::~MainWindow()
{
    delete camera;
    delete ui;
}

