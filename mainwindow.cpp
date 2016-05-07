#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    // camera properties for calculated distance
    focal_length(CAMERA_FOCAL_LENGTH_MM),
    baseline_mm(CAMERA_BASELINE_MM),
    frameThread(NULL)
{
    ui->setupUi(this);
    this->setWindowTitle("ZPO 2016");
    createMenu();

    distanceQueue = new ThreadSafeQueue<QImage>();
    depthQueue = new ThreadSafeQueue<QImage>();
    renderingPoints = new QList<QSharedPointer<DistancePoint>>();
    // widget for measuring distance
    glDistanceWidget = new GLWidget(renderingPoints, distanceQueue, this);
    ui->glDistanceLayout->addWidget(glDistanceWidget,0,0);
    // widget for showing depth map
    glDepthWidget = new GLWidget(NULL, depthQueue, this);
    ui->glDepthLayout->addWidget(glDepthWidget,0,0);
    // prepared lookup table for depth map
    colorLut = cv::Mat(cv::Size(256, 1), CV_8UC3);
    prepareColorLut(&colorLut);

    //connect signals
    connect(glDistanceWidget,SIGNAL(measuringPointCoordsChanged(QPoint, QSize)),this,SLOT(onMeasuringPointCoordsChanged(QPoint, QSize)));
    connect(this,SIGNAL(newDistanceFrame()),glDistanceWidget,SLOT(onNewFrame()));
    connect(this,SIGNAL(newDepthFrame()), glDepthWidget, SLOT(onNewFrame()));

    // connect and setup camera
   setUpCamera();
}

/**
 * Prepares cameras options
 * @brief MainWindow::setUpCamera
 */
void MainWindow::setUpCamera()
{
    try{
        if(camera == NULL)
            camera = new StereoCamera(CAMERA_WIDTH, CAMERA_HEIGHT, CAMERA_FPS, LICENSE);

        camera->open();
        camera->setParams();
        disparities = (camera->getParams().numDisparities * 16);

        // Set exposure, LED brightness and camera orientation
        camera->setLed(30);
        camera->setGain(34);
        camera->setExposure(45);
        camera->setUndistort(true);
        camera->setVerticalFlip(true);

        camera->start(newFrameCallback, this);

        this->ui->led_val->setText(QString::number(camera->getLed(),'f', 2) + " %");
        this->ui->gain_val->setText(QString::number(camera->getGain(),'f', 2) + " %");
        this->ui->exposure_val->setText(QString::number(camera->getExposure(),'f', 2) + " %");

    } catch(std::invalid_argument * error){
        QMessageBox::warning(this, "Invalid argument", error->what());
        delete camera;
        camera = NULL;
    }
}

/**
 * Callback function for camera. Camera sends frame (one by one) into this method
 * @brief MainWindow::onNewFrame
 * @param pFrameData
 */
void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    if(!_mutex.tryLock(10)) return;

    if(ui->tabWidget->currentIndex() == 0){
        distanceCalculation(pFrameData);
    }
    else{
        depthCalculation(pFrameData);
    }

    _mutex.unlock();
}

/**
 * Calculating and rendering distance data
 * @brief MainWindow::distanceCalculation
 * @param pFrameData frame got from DUO camera
 */
void inline MainWindow::distanceCalculation(const PDense3DFrame pFrameData){
    cv::Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);

    for(int i = 0; i < renderingPoints->length(); i++){
        QSharedPointer<DistancePoint> distancePoint = renderingPoints->at(i);

        float distance = 0;
        // distance from depth map
        if(ui->buildMeasuring->isChecked()){
            cv::Mat depthMat = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);
            cv::Vec3f point = depthMat.at<cv::Vec3f>(distancePoint->y, distancePoint->x);
            distance = point[2];
            distance /= 10.0; // normalize to cm
        }
        // calculated distance
        if(ui->computedMeasuring->isChecked()){
            cv::Mat disparityMat = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
            distance = baseline_mm * focal_length / disparityMat.at<float>(distancePoint->y, distancePoint->x);
            distance *= 10.0; // normalize to cm
        }

        distancePoint->distance = distance;
    }

    cv::Mat leftCamFrame = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    QImage frame = QImage(leftCamFrame.data, leftCamFrame.cols, leftCamFrame.rows, QImage::Format_Grayscale8);

    distanceQueue->enqueue(frame);
    emit newDistanceFrame();
}

/**
 * Calculating depth map from disparity map
 * @brief MainWindow::depthCalculation
 * @param pFrameData frame from DUO camera
 */
void inline MainWindow::depthCalculation(const PDense3DFrame pFrameData){
    cv::Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    cv::Mat disparityMat = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);

    cv::Mat disp8, rgbDisparity;

    disparityMat.convertTo(disp8, CV_8UC1, 255.0 / disparities);
    cv::cvtColor(disp8, rgbDisparity, cv::COLOR_GRAY2BGR);
    //color depth map
    cv::LUT(rgbDisparity, colorLut, _depthRGB);

    QImage frame = QImage(_depthRGB.data, _depthRGB.cols, _depthRGB.rows, QImage::Format_RGB888);
    depthQueue->enqueue(frame);
    emit newDepthFrame();
}

/**
 * Event from widget saying it was clicked on it (adding/replacing points)
 * @brief MainWindow::onMeasuringPointCoordsChanged
 * @param pos
 * @param widgetSize
 */
void MainWindow::onMeasuringPointCoordsChanged(QPoint pos, QSize widgetSize)
{
    // single rendering point
    if(ui->singleMeasuringPoint->isChecked()){
        renderingPoints->clear();
    }

    int frameX = (pos.x() / (double) widgetSize.width()) * CAMERA_WIDTH;
    int frameY = (pos.y() / (double) widgetSize.height()) * CAMERA_HEIGHT;

    renderingPoints->append(QSharedPointer<DistancePoint>(new DistancePoint(pos, widgetSize, frameX, frameY)));
}

void MainWindow::on_connectCameraButton_clicked()
{
    if(this->camera != NULL){
        QMessageBox::warning(this, "Camera connected", "DUO Camera is already connected!");
        return;
    }

    setUpCamera();
}

void MainWindow::on_clearPoints_clicked()
{
    renderingPoints->clear();
}

void MainWindow::on_ledSlider_valueChanged(int value)
{
    camera->setLed(value * 0.1);
    this->ui->led_val->setText(QString::number(camera->getLed(),'f', 2) + " %");
}

void MainWindow::on_gainSlider_valueChanged(int value)
{
    camera->setGain(value * 0.1);
    this->ui->gain_val->setText(QString::number(camera->getGain(),'f', 2) + " %");
}

void MainWindow::on_exposureSlider_valueChanged(int value)
{
    camera->setExposure(value * 0.1);
    this->ui->exposure_val->setText(QString::number(camera->getExposure(),'f', 2) + " %");
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

/**
 * Creates UI menu
 * @brief MainWindow::createMenu
 */
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

/**
  Destructor closes cameras and dense
 * @brief MainWindow::~MainWindow
 */
MainWindow::~MainWindow()
{
    _mutex.lock();
    delete camera;
    _mutex.unlock();

    delete ui;
    delete renderingPoints;
    delete depthQueue;
    delete distanceQueue;
}
