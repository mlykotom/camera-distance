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
    params.sadWindowSize = 3;
    params.numDisparities = 9;
    params.preFilterCap = 63;
    params.uniqenessRatio = 10;
    params.speckleWindowSize = 100;
    params.speckleRange = 32;
    if (!SetDense3Params(dense, params)) {
        Dense3DClose(dense);
        throw new std::invalid_argument("GetDense3Params error");
    }

    SetDUOResolutionInfo(duo, resolutionInfo);
    uint32_t w, h;
    GetDUOFrameDimension(duo, &w, &h);
    qDebug() << "Frame Dimension: [" << w << "," << h << "]";

    // Set exposure, LED brightness and camera orientation
    this->setLed(45);
    this->setGain(0);
    this->setExposure(50);
    this->setUndistort(true);
    this->setVerticalFlip(false);
}

StereoCamera::~StereoCamera(){
    if(duo != NULL){
        CloseDUO(duo);
        StopDUO(duo);
    }

    if(dense != NULL){
        Dense3DClose(dense);
        Dense3DStop(dense);
    }
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
