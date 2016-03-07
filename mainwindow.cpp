#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "utils.h"

#define WIDTH 320
#define HEIGHT 240
#define FPS 60

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
//    ui(new Ui::MainWindow),
    _duo(NULL), _dense(NULL)
{
//    ui->setupUi(this);
    resize(320*2, 240);
    setWindowTitle("DUO Test");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QSplitter *hs = new QSplitter();
       hs->addWidget(_img[0] = new ImageOutput());
       hs->addWidget(_img[1] = new ImageOutput());
       hs->addWidget(_img[2] = new ImageOutput());
       setCentralWidget(hs);

//       StereoCamera camera = new StereoCamera(WIDTH, HEIGHT, FPS);

       this->colorLut = Mat(cv::Size(256, 1), CV_8UC3);
       prepareColorLut(&colorLut);

      DUOResolutionInfo ri;
      if(EnumerateResolutions(&ri, 1, WIDTH, HEIGHT, DUO_BIN_HORIZONTAL2 + DUO_BIN_VERTICAL2, FPS))
      {
          printf("[%dx%d], [%f-%f], %f, [%d]", ri.width, ri.height, ri.minFps, ri.maxFps, ri.fps, ri.binning);

          if(OpenDUO(&_duo)){
              printf("Could not open DUO camera\n");
              close(); // TODO
          }

          char buf[256];
          GetDUOSerialNumber(_duo, buf);
          qDebug() << "Serial Number: " << buf;
          GetDUOFirmwareVersion(_duo, buf);
          qDebug() << "Firmware Version: v" << buf;
          GetDUOFirmwareBuild(_duo, buf);
          qDebug() << "Firmware Build Time: " << buf;
          qDebug() << "Library Version: v" << GetLibVersion();
          qDebug() << "Dense3DMT Version: v" << Dense3DGetLibVersion();

          // Open Dense3D
          if (!Dense3DOpen(&_dense, _duo)) {
              printf("Could not open Dense3DMT library\n");
              close(); // TODO
          }

          // Set the Dense3D license
          if (!SetDense3DLicense(_dense, LICENSE)) // <-- Put your Dense3D license
          {
              printf("Invalid or missing Dense3D license. To get your license visit https://duo3d.com/account\n");
              // Close Dense3D library
              Dense3DClose(_dense);
              close(); // TODO
          }

          // Set the image size
          if (!SetDense3DImageInfo(_dense, WIDTH, HEIGHT, FPS)) {
              printf("Invalid image size\n");
              // Close Dense3D library
              Dense3DClose(_dense);
              close(); // TODO
          }

          // Set Dense3D parameters
          Dense3DParams params;
          params.scale = 0;
          params.mode = 0;
          params.numDisparities = 2;
          params.sadWindowSize = 6;
          params.preFilterCap = 28;
          params.uniqenessRatio = 27;
          params.speckleWindowSize = 52;
          params.speckleRange = 14;
          if (!SetDense3Params(_dense, params)) {
              printf("GetDense3Params error\n");
              // Close Dense3D library
              Dense3DClose(_dense);
              close(); // TODO
          }

          // setup callback
          Dense3DStart(_dense, newFrameCallback, this);

          SetDUOResolutionInfo(_duo, ri);
          uint32_t w, h;
          GetDUOFrameDimension(_duo, &w, &h);
          qDebug() << "Frame Dimension: [" << w << "," << h << "]";

          SetDUOUndistort(_duo,false);
          SetDUOLedPWM(_duo, 80);
          SetDUOGain(_duo, 50);
          SetDUOExposure(_duo, 50);
          SetDUOVFlip(_duo, false);
      }
  }


void MainWindow::onNewFrame(const PDense3DFrame pFrameData){
    Frame3D frame;
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
    frame.disparity.convertTo(disp8, CV_8UC1, 255.0/(2*16)); //params.numDisparities instead of 2
    Mat rgbBDisparity;
    cvtColor(disp8, rgbBDisparity, COLOR_GRAY2BGR);
    cv::LUT(rgbBDisparity, colorLut, rgbBDisparity);

    Q_EMIT _img[2]->setImage(rgbBDisparity);
}

MainWindow::~MainWindow()
{
//    delete ui;
    if(_duo) CloseDUO(_duo);
    Dense3DStop(_dense);
    Dense3DClose(_dense);
}

void MainWindow::closeEvent(QCloseEvent *event){

}
