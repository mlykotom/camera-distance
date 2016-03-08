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

    unsigned gain;
    unsigned exposure;
    unsigned led;

    DUOResolutionInfo resolutionInfo;
    Dense3DParams params;
public:
    StereoCamera(unsigned initWidth, unsigned initHeight, unsigned initFps, const std::string license):
        width(initWidth), height(initHeight), fps(initFps)
    {
        // Find optimal binning parameters for given (width, height)
        // This maximizes sensor imaging area for given resolution
        int binning = DUO_BIN_NONE;
        if(width <= 752/2)
            binning += DUO_BIN_HORIZONTAL2;
        else if(width <= 752/4)
            binning += DUO_BIN_HORIZONTAL4;
        if(height <= 480/4)
            binning += DUO_BIN_VERTICAL4;
        else if(height <= 480/2)
            binning += DUO_BIN_VERTICAL2;

        if(!EnumerateResolutions(&resolutionInfo, 1, width, height, binning, fps)){
            throw new std::invalid_argument("Could not enumerate resolutions");
        }

        if(!OpenDUO(&duo)){
            throw new std::invalid_argument("Could not open DUO camera");
        }

        if (!Dense3DOpen(&dense, duo)) {
            throw new std::invalid_argument("Could not open Dense3DMT library");
        }

        // Set the Dense3D license
        if (!SetDense3DLicense(dense, license.c_str()))
        {
            Dense3DClose(dense);
            throw new std::invalid_argument("Invalid or missing Dense3D license. To get your license visit https://duo3d.com/account");
        }

        // Set the image size
        if (!SetDense3DImageInfo(dense, width, height, fps)) {
            Dense3DClose(dense);
            throw new std::invalid_argument("Invalid image size");
        }

        printInfo();
    }

    // TODO
    void setParams(){
        params.scale = 0;
        params.mode = 3;
        params.numDisparities = 2;
        params.sadWindowSize = 6;
        params.preFilterCap = 28;
        params.uniqenessRatio = 27;
        params.speckleWindowSize = 52;
        params.speckleRange = 14;
        if (!SetDense3Params(dense, params)) {
            Dense3DClose(dense);
            throw new std::invalid_argument("GetDense3Params error");
        }

        SetDUOResolutionInfo(duo, resolutionInfo);
        uint32_t w, h;
        GetDUOFrameDimension(duo, &w, &h);
        qDebug() << "Frame Dimension: [" << w << "," << h << "]";

        // Set exposure, LED brightness and camera orientation
        SetDUOExposure(duo, 40);
        SetDUOLedPWM(duo, 50);
        SetDUOGain(duo, 30);
        SetDUOVFlip(duo, false);
        SetDUOUndistort(duo, false);
    }

    Dense3DParams getParams(){
        return params;
    }

    void start(Dense3DFrameCallback callback, void *userData){
        Dense3DStart(dense, callback, userData);
    }

    ~StereoCamera(){
        Dense3DStop(dense);
        Dense3DClose(dense);
        CloseDUO(duo);
    }

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

    void printInfo(){
        char buf[256];
        GetDUOSerialNumber(duo, buf);
        qDebug() << "Serial Number: " << buf;
        GetDUOFirmwareVersion(duo, buf);
        qDebug() << "Firmware Version: v" << buf;
        GetDUOFirmwareBuild(duo, buf);
        qDebug() << "Firmware Build Time: " << buf;
        qDebug() << "Library Version: v" << GetLibVersion();
        qDebug() << "Dense3DMT Version: v" << Dense3DGetLibVersion();
    }
};

#endif // STEREOCAMERA_H
