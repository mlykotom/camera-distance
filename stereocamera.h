#ifndef STEREOCAMERA_H
#define STEREOCAMERA_H

#include "DUOLib.h"
#include "Dense3DMT.h"

class StereoCamera
{
private:
    DUOInstance duo;
    Dense3DMTInstance dense;

    unsigned width;
    unsigned height;
    unsigned fps;
public:
    StereoCamera(unsigned width, unsigned height, unsigned fps);
    DUOInstance getCamera(){
        return duo;
    }
};

#endif // STEREOCAMERA_H
