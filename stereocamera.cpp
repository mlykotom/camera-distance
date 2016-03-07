#include "stereocamera.h"

StereoCamera::StereoCamera(unsigned width, unsigned height, unsigned fps){

}

StereoCamera::~StereoCamera(){
    Dense3DStop(dense);
    Dense3DClose(dense);
    CloseDUO(duo);
}
