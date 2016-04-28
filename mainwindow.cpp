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

    glDistanceWidget = new GLWidget(true,distancesList, this);
    ui->glDistanceLayout->addWidget(glDistanceWidget,0,0);

    glDepthWidget = new GLWidget(false,NULL, this);
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
    connect(this->ui->multipleMeasuringPoints,SIGNAL(toggled(bool)),
            glDistanceWidget,SLOT(onNumberOfMeasuringPointsChanged(bool)));
    connect(ui->connectCameraButton,SIGNAL(clicked()),this,SLOT(setUpCamera()));
}



void MainWindow::setUpCamera()
{
    try{
        if(camera == NULL)
            camera = new StereoCamera(WIDTH, HEIGHT, FPS, LICENSE);

        camera->open();
        camera->setParams();
        camera->start(newFrameCallback, this);
    } catch(std::invalid_argument * error){
        QMessageBox::warning(this, "Invalid argument", error->what());
    }
}

void MainWindow::onNewFrame(const PDense3DFrame pFrameData){

    if(!_mutex.tryLock(10)) return;

  //  queue->enqueue(1);
//    if(queue->length() > 5) return;

    D3DFrame frame;
    Size frameSize(pFrameData->duoFrame->width,pFrameData->duoFrame->height);
    frame.leftImg = cv::Mat(frameSize, CV_8U, pFrameData->duoFrame->leftData);
    frame.disparity = cv::Mat(frameSize, CV_32F, pFrameData->disparityData);
    frame.depth = cv::Mat(frameSize, CV_32FC3, pFrameData->depthData);


    if(ui->tabWidget->currentIndex() == 0){
        cvtColor(frame.leftImg, _leftRGB, COLOR_GRAY2BGR);

        float distance = 0;

        //multiple measuring points
        if(ui->multipleMeasuringPoints->isChecked()){
            for(int i = 0; i <  measuringPointsList.length() ; i++){
                // distance from depth map
                if(ui->buildMeasuring->isChecked())
                    distance = buildInDistance(frame.depth.at<Vec3f>(measuringPointsList.at(i)));

                // calculated distance
                if(ui->computedMeasuring->isChecked())
                    distance = computedDistance(frame.disparity.at<float>(measuringPointsList.at(i)));

                distancesList->insert(i,QString::number(distance,'f',2));

                glDistanceWidget->setImage(_leftRGB);
            }
        }
        //single measuring point
        else{
            // distance from depth map
            if(ui->buildMeasuring->isChecked())
                distance = buildInDistance(frame.depth.at<Vec3f>(p));

            // calculated distance
            if(ui->computedMeasuring->isChecked())
                distance = computedDistance(frame.disparity.at<float>(p));

            //render
            glDistanceWidget->setImage(_leftRGB,distance);
        }
        //show distance
        //ui->depth->setText(QString::number(distance));

    }
    else if(ui->tabWidget->currentIndex() == 1){
        int disparities = (camera->getParams().numDisparities * 16);
        Mat disp8, rgbBDisparity;
        frame.disparity.convertTo(disp8, CV_8UC1, 255.0 / disparities);
        cv::cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
        //color depth map
        cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);
        //render
        glDepthWidget->setImage(rgbBDisparity,0);
    }

   // ui->depth->setText(QString::number(queue->length()));

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


float MainWindow::buildInDistance(Vec3f chro)
{
    //cv::Vec3f chro = frame.depth.at<Vec3f>(p);
    return (chro[2] / 10.0);
}

float MainWindow::computedDistance(float disparity)
{
    return ((baseline_mm * focal_length_pixels / disparity) / 100.0);
}





MainWindow::~MainWindow()
{
    //tu to pada pri zavreni okna
    //    if (camera != NULL)
    //            delete camera;
    delete distancesList;
    delete ui;
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
    qDebug() << camera->getVerticalFlip();
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
*   TODO:
*
*/
