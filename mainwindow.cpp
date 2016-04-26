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

    glWidget = new GLWidhget();
    this->setCentralWidget(glWidget);

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

    connect(ui->out_left,SIGNAL(measuringPointCoordsChanged(int,int)),this,SLOT(onMeasuringPointCoordsChanged(int,int)));

    //_mutex = new QMutex();
   //_mutex->unlock();

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
        //nejde :(
        //        frame.depth = cv::Mat3f(frameSize, CV_32FC3, pFrameData->depthData);

        frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);

        int disparities = (camera->getParams().numDisparities * 16);

        //Point p = Point(centerX, centerY);

        cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);

        //        cv::Rect testRect = cv::Rect_<int>(centerX - 10, centerY - 10, 20, 20);
        //        _leftRGB(testRect) = 0xFF0000;

        cv::Vec3f chro = frame.depth.at<Vec3f>(p);
        float depthPoint = chro[2] / 10.0;

        float focal_length_pixels = 2 * 360;
        float baseline_mm = 30;
        float depthCalculated = (baseline_mm * focal_length_pixels /
                                 frame.disparity.at<float>(p)) / 100.0;

//        ui->depth->setText(QString::number(depthCalculated) + " | " + QString::number(depthPoint));


//        Mat disp8, rgbBDisparity;
//        frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / disparities);
//        cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);

        //farebna hlbkova mapa
        //        cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);

        //zobrazenie
        // druhy parameter je vzdialenost -> depthCalculated, 69 hihihihih
       // ui->out_left->setImage(_leftRGB,69.6969);
        //ui->out_depth->setImage(rgbBDisparity,0);
        glWidget->setImage(_leftRGB,0);

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
