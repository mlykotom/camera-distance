#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "DUOLib.h"
#include "Dense3DMT.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <QDebug>
#include "distance_point.h"

class StereoCamera
{
private:
    DUOInstance duo;
    Dense3DMTInstance dense;

    unsigned width;
    unsigned height;
    unsigned fps;
    std::string license;

    DUOResolutionInfo resolutionInfo;
    Dense3DParams params;
public:
    StereoCamera(unsigned initWidth, unsigned initHeight, unsigned initFps, const std::string initLicense);
    ~StereoCamera();

    void open();
    void stop();
    void setParams();

    Dense3DParams getParams();

    void start(Dense3DFrameCallback callback, void *userData){
        Dense3DStart(dense, callback, userData);
    }

    DUOInstance getCamera(){
        return duo;
    }

    Dense3DMTInstance getDense(){
        return dense;
    }

    void setExposure(double value);
    double getExposure();

    void setVerticalFlip(bool isSet);
    bool getVerticalFlip();

    void setUndistort(bool isSet);
    bool getUndistort();

    void setGain(double value);
    double getGain();

    void setLed(double value);
    double getLed();

    void printInfo();
};

#endif // STEREOCAMERA_H
