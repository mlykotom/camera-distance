#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T14:39:04
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = camera-distance
TEMPLATE = app

CONFIG += c++11

win32{

}
macx{
 OPENCV_INC = /usr/local/include/
 OPENCV_LIBS = -L/usr/local/lib \
		-lopencv_core \
		-lopencv_highgui \
		-lopencv_imgproc \
		-lopencv_features2d \
		-lopencv_calib3d

 DUO_INC = $$PWD/SDK/include
 DUO_LIBS = -L$$PWD/SDK/osx/x64/
}

# OpenCV dependency
INCLUDEPATH += $${OPENCV_INC}
LIBS += $${OPENCV_LIBS}
# DUO3D dependency
INCLUDEPATH += $${DUO_INC}
LIBS += $${DUO_LIBS} \
		-lDUO \
		-lDense3DMT

SOURCES += main.cpp\
		mainwindow.cpp \
	stereocamera.cpp

HEADERS  += api_keys.h mainwindow.h stereocamera.h image_output.h \
	utils.h

FORMS    += mainwindow.ui

