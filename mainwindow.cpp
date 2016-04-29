#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

#define WIDTH 320
#define HEIGHT 240
#define FPS 40

int centerX = WIDTH / 2;
int centerY = HEIGHT / 2;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    camera(NULL)
{
    ui->setupUi(this);
    this->setWindowTitle("ZPO 2016");
    createMenu();

    distancesList = new QList<QString>();

    distanceQueue = new QQueue<QImage>();
    depthQueue = new QQueue<QImage>();

    glDistanceWidget = new GLWidget(true,distancesList,distanceQueue, this);
    ui->glDistanceLayout->addWidget(glDistanceWidget,0,0);

    glDepthWidget = new GLWidget(false,NULL,depthQueue, this);
    ui->glDepthLayout->addWidget(glDepthWidget,0,0);

    colorLut = Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    try{
        camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);
        setUpCamera();
    } catch(std::invalid_argument * error){
        QMessageBox::warning(this, "Invalid argument", error->what());
    }

    p = cv::Point(centerX, centerY);

    // camera properties for calculated distance
    focal_length_pixels = 2 * 360;
    baseline_mm = 30;

    //connect signlas
    connect(glDistanceWidget,SIGNAL(measuringPointCoordsChanged(int,int)),this,SLOT(onMeasuringPointCoordsChanged(int,int)));
    connect(this->ui->multipleMeasuringPoints,SIGNAL(toggled(bool)), glDistanceWidget,SLOT(onNumberOfMeasuringPointsChanged(bool)));
    connect(ui->connectCameraButton,SIGNAL(clicked()),this,SLOT(setUpCamera()));
    connect(ui->clearPoints,SIGNAL(clicked()),glDistanceWidget,SLOT(onPointsClear()));
    connect(this,SIGNAL(newDistanceFrame()),glDistanceWidget,SLOT(onNewFrame()));
    connect(this,SIGNAL(newDepthFrame()),glDepthWidget,SLOT(onNewFrame()));
}



void MainWindow::setUpCamera()
{
    try{
        if(camera == NULL)
            camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);

        camera->open();
        camera->setParams();
        disparities = (camera->getParams().numDisparities * 16);
        camera->start(newFrameCallback, this);
    } catch(std::invalid_argument * error){
        QMessageBox::warning(this, "Invalid argument", error->what());
    }


}

void inline MainWindow::distanceCalculation(const PDense3DFrame pFrameData){
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);

    //single measuring point
    float distance = 0;
    // distance from depth map
    if(ui->buildMeasuring->isChecked()){
        cv::Mat depthMat = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);
        distance = depthMat.at<Vec3f>(p)[2] / 10.0;
    }
    // calculated distance
    if(ui->computedMeasuring->isChecked()){
        cv::Mat disparityMat = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
        distance = ((baseline_mm * focal_length_pixels / disparityMat.at<float>(p)) / 100.0);
    }

    cv::Mat leftCamFrame = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    distanceQueue->enqueue(QImage(leftCamFrame.data, leftCamFrame.cols, leftCamFrame.rows, QImage::Format_Grayscale8));
    emit newDistanceFrame();
    //show distance
    ui->depth->setText(QString::number(distance));

    //multiple measuring points
//    if(ui->multipleMeasuringPoints->isChecked()){
//        for(int i = 0; i <  measuringPointsList.length() ; i++){
//            // distance from depth map
//            if(ui->buildMeasuring->isChecked())
//                distance = buildInDistance(frame.depth.at<Vec3f>(measuringPointsList.at(i)));

//            // calculated distance
//            if(ui->computedMeasuring->isChecked())
//                distance = computedDistance(frame.disparity.at<float>(measuringPointsList.at(i)));

//            distancesList->insert(i,QString::number(distance,'f',2));

//            //                glDistanceWidget->setImage(_leftRGB);
//        }
//    }
}

void inline MainWindow::depthCalculation(const PDense3DFrame pFrameData){
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    cv::Mat disparityMat = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);

    Mat disp8, rgbDisparity;
    disparityMat.convertTo(disp8, CV_8UC1, 255.0 / disparities);
    cv::cvtColor(disp8, rgbDisparity, COLOR_GRAY2BGR);
    //color depth map
    cv::LUT(rgbDisparity, colorLut, _depthRGB);

    depthQueue->enqueue(QImage(_depthRGB.data, _depthRGB.cols, _depthRGB.rows, QImage::Format_RGB888));
    emit newDepthFrame();
}

/**
 * Callback function for camera. Camera sends frame (one by one) into this method
 * @brief MainWindow::onNewFrame
 * @param pFrameData
 */
void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    _mutex.lock();

    if(ui->tabWidget->currentIndex() == 0){
        distanceCalculation(pFrameData);
    }
    else{
        depthCalculation(pFrameData);
    }

    _mutex.unlock();
}


void MainWindow::createMenu()
{
    QAction *closeAct = new QAction(tr("&Close"), this);
    closeAct->setStatusTip(tr("Close program"));
    connect(closeAct, &QAction::triggered, this, &MainWindow::close);

    QMenu *fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(closeAct);

    QAction *authorsAct = new QAction(tr("&Authors"), this);
    authorsAct->setStatusTip(tr("About Authors"));
    connect(authorsAct, SIGNAL(triggered()), this, SLOT(showAuthorsDialog()));

    QAction *aboutAct = new QAction(tr("&About"), this);
    aboutAct->setStatusTip(tr("About Qt"));
    connect(aboutAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(authorsAct);
    helpMenu->addAction(aboutAct);
}


QThread *MainWindow::getMainThread()
{
    return qApp->thread();
}

void MainWindow::onMeasuringPointCoordsChanged(int x, int y)
{
    //multiple measuring points
    if(ui->multipleMeasuringPoints->isChecked()){
        distancesList->append("");
        measuringPointsList.append(Point(x, y));
    }
    //single measuring point
    else
        p = Point(x, y);
}

void MainWindow::on_ledSlider_valueChanged(int value)
{
    camera->setLed(value*0.1);
}

void MainWindow::on_gainSlider_valueChanged(int value)
{
    camera->setGain(value*0.1);
}

void MainWindow::on_exposureSlider_valueChanged(int value)
{
    camera->setExposure(value*0.1);
}

void MainWindow::on_swapVerticalCheckbox_clicked(bool checked)
{
    camera->setVerticalFlip(checked);
}

void MainWindow::showAuthorsDialog()
{
    QMessageBox msgBox;
    msgBox.setWindowTitle("Authors");
    msgBox.setText("Authors:\n"
                   "Roman Čižmarik, xcizma04 \n"
                   "Tomáš Mlynarič, xmylna06");
    msgBox.exec();
}

MainWindow::~MainWindow()
{
    if (camera != NULL){
//        delete camera;
        // TODO
    }
    delete distancesList;
    delete ui;
}
