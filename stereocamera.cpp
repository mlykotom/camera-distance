#include "stereocamera.h"

StereoCamera::StereoCamera(unsigned initWidth, unsigned initHeight, unsigned initFps, const std::string initLicense):
    width(initWidth), height(initHeight), fps(initFps), license(initLicense), duo(NULL), dense(NULL)
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
}

void StereoCamera::open(){
    if(!OpenDUO(&duo)){
        throw new std::invalid_argument("Could not open DUO camera");
    }

    if (!Dense3DOpen(&dense, duo)) {
        throw new std::invalid_argument("Could not open Dense3DMT library");
    }

    // Set the Dense3D license
    if (!SetDense3DLicense(dense, license.c_str()))
    {
        throw new std::invalid_argument("Invalid or missing Dense3D license. To get your license visit https://duo3d.com/account");
    }

    // Set the image size
    if (!SetDense3DImageInfo(dense, width, height, fps)) {
        throw new std::invalid_argument("Invalid image size");
    }
}

void StereoCamera::setParams(){
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
}

Dense3DParams StereoCamera::getParams()
{
    return params;
}

void StereoCamera::setExposure(double value)
{
    SetDUOExposure(duo, value);
}

double StereoCamera::getExposure()
{
    double value;
    GetDUOExposure(duo, &value);
    return value;
}

void StereoCamera::setVerticalFlip(bool isSet)
{
    SetDUOVFlip(duo, isSet);
}

bool StereoCamera::getVerticalFlip()
{
    int value = false;
    GetDUOVFlip(duo, &value);
    return value;
}

void StereoCamera::setUndistort(bool isSet)
{
    SetDUOUndistort(duo, isSet);
}

bool StereoCamera::getUndistort()
{
    bool value = false;
    GetDUOUndistort(duo, &value);
    return value;
}

void StereoCamera::setGain(double value)
{
    SetDUOGain(duo, value);
}

double StereoCamera::getGain()
{
    double value;
    GetDUOGain(duo, &value);
    return value;
}

void StereoCamera::setLed(double value)
{
    SetDUOLedPWM(duo, value);
}

double StereoCamera::getLed()
{
    double value;
    !GetDUOLedPWM(duo, &value);
    return value;
}

void StereoCamera::stop(){
    StopDUO(duo);
    Dense3DStop(dense);
}

StereoCamera::~StereoCamera(){
    this->stop();
    CloseDUO(duo);
    Dense3DClose(dense);
}

void StereoCamera::printInfo(){
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
