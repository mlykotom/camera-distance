#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "DUOLib.h"
#include "Dense3DMT.h"
#include <opencv2/opencv.hpp>
#include <stdexcept>
#include <QDebug>

struct D3DFrame {
    cv::Mat leftImg, rightImg;
    cv::Mat disparity;
    cv::Mat depth;
};

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

    // TODO
    void setParams();

    Dense3DParams getParams(){
        return params;
    }

    //prerobit na connect
    void start(Dense3DFrameCallback callback, void *userData){
        Dense3DStart(dense, callback, userData);
    }

    DUOInstance getCamera(){
        return duo;
    }

    Dense3DMTInstance getDense(){
        return dense;
    }

    void setExposure(double value) {
        SetDUOExposure(duo, value);
    }

    double getExposure(){
        double value;
        GetDUOExposure(duo, &value);
        return value;
    }

    void setVerticalFlip(bool isSet) {
        SetDUOVFlip(duo, isSet);
    }

    bool getVerticalFlip(){
        int value = false;
        GetDUOVFlip(duo, &value);
        return value;
    }

    void setUndistort(bool isSet) {
        SetDUOUndistort(duo, isSet);
    }

    bool getUndistort(){
        bool value = false;
        GetDUOUndistort(duo, &value);
        return value;
    }

    void setGain(double value) {
        SetDUOGain(duo, value);
    }

    double getGain(){
        double value;
        GetDUOGain(duo, &value);
        return value;
    }

    void setLed(double value) {
        SetDUOLedPWM(duo, value);
    }

    double getLed(){
        double value;
        GetDUOLedPWM(duo, &value);
        return value;
    }

    void printInfo();
};

#endif // STEREOCAMERA_H
