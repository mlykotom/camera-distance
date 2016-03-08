#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
//    ui(new Ui::MainWindow),
    _duo(NULL)
{
//    ui->setupUi(this);
    resize(320*2, 240);
    setWindowTitle("DUO Test");
    setGeometry(QStyle::alignedRect(Qt::LeftToRight, Qt::AlignCenter, size(), qApp->desktop()->availableGeometry()));
    QSplitter *hs = new QSplitter();
       hs->addWidget(_img[0] = new ImageOutput());
       hs->addWidget(_img[1] = new ImageOutput());
       setCentralWidget(hs);

    DUOResolutionInfo ri;
      if(EnumerateResolutions(&ri, 1, 320, 240, DUO_BIN_HORIZONTAL2 + DUO_BIN_VERTICAL2, 60))
      {
          printf("[%dx%d], [%f-%f], %f, [%d]", ri.width, ri.height, ri.minFps, ri.maxFps, ri.fps, ri.binning);

          if(OpenDUO(&_duo))
          {
              char buf[256];
              GetDUOSerialNumber(_duo, buf);
              printf("Serial Number: %s", buf);
              GetDUOFirmwareVersion(_duo, buf);
              printf("Firmware Version: v%s", buf);
              GetDUOFirmwareBuild(_duo, buf);
              printf("Firmware Build Time: %s", buf);
              printf("Library Version: v%s", GetLibVersion());
              printf("-------------------------------------------------");

              SetDUOResolutionInfo(_duo, ri);
              uint32_t w, h;
              GetDUOFrameDimension(_duo, &w, &h);
              printf("Frame Dimension: [%d, %d]", w, h);

              StartDUO(_duo, newFrameCb, this);
              SetDUOLedPWM(_duo, 30);
              SetDUOGain(_duo, 0);
              SetDUOExposure(_duo, 50);
              SetDUOVFlip(_duo, true);
          }
      }
  }

void MainWindow::onNewFrame(const PDUOFrame pFrameData){
    Mat left(Size(pFrameData->width,pFrameData->height), CV_8UC1, pFrameData->leftData);
    Mat right(Size(pFrameData->width,pFrameData->height), CV_8UC1, pFrameData->rightData);
    cvtColor(left, _leftRGB, COLOR_GRAY2BGR);
    cvtColor(right, _rightRGB, COLOR_GRAY2BGR);

    Q_EMIT _img[0]->setImage(_leftRGB);
    Q_EMIT _img[1]->setImage(_rightRGB);
}

MainWindow::~MainWindow()
{
//    delete ui;
    if(_duo) CloseDUO(_duo);
}
