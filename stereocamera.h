#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "DUOLib.h"
#include "Dense3DMT.h"
#include <opencv2/opencv.hpp>

struct Frame3D {
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

    unsigned gain;
    unsigned exposure;
    unsigned led;

public:
    StereoCamera(unsigned width, unsigned height, unsigned fps);
    ~StereoCamera();

//    bool open(){
//        return OpenDUO(duo);
//    }

    DUOInstance getCamera(){
        return duo;
    }

    Dense3DMTInstance getDense(){
        return dense;
    }

    void setExposure(float value) {
        SetDUOExposure(duo, value);
    }

    void setUndistort(bool isSet) {
        SetDUOUndistort(duo, isSet);
    }

    void setGain(float value) {
        SetDUOGain(duo, value);
    }

    void setLed(float value) {
        SetDUOLedPWM(duo, value);
    }

};

#endif // STEREOCAMERA_H
