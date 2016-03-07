#ifndef UTILS_H
#define UTILS_H

#include <opencv2/opencv.hpp>

cv::Vec3b HSV2RGB(float hue, float sat, float val) {
    float x, y, z;

    if (hue == 1) hue = 0;
    else hue *= 6;

    int i = (int) floorf(hue);
    float f = hue - i;
    float p = val * (1 - sat);
    float q = val * (1 - (sat * f));
    float t = val * (1 - (sat * (1 - f)));

    switch (i) {
        case 0:
            x = val;
            y = t;
            z = p;
            break;
        case 1:
            x = q;
            y = val;
            z = p;
            break;
        case 2:
            x = p;
            y = val;
            z = t;
            break;
        case 3:
            x = p;
            y = q;
            z = val;
            break;
        case 4:
            x = t;
            y = p;
            z = val;
            break;
        case 5:
            x = val;
            y = p;
            z = q;
            break;
        default:
            return cv::Vec3b(0, 0, 0);
    }
    return cv::Vec3b((uchar) (x * 255), (uchar) (y * 255), (uchar) (z * 255));
}


void prepareColorLut(cv::Mat *colorLut) {
    for (int i = 0; i < 256; i++) {
        colorLut->at<cv::Vec3b>(i) = i == 0 ? cv::Vec3b(0, 0, 0) : HSV2RGB(i / 256.0f, 1, 1);
    }
}

#endif // UTILS_H
