#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T14:39:04
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = camera-distance
TEMPLATE = app

CONFIG += c++11

# opencv
INCLUDEPATH += C:\opencv\build\include
LIBS += -LC:\opencv\build\x86\vc11\lib \
        -lopencv_core2412 \
        -lopencv_highgui2412 \
        -lopencv_imgproc2412 \
        -lopencv_features2d2412 \
        -lopencv_calib3d2412

# DUO3D dependency
INCLUDEPATH +=  C:\Users\Roman\Documents\DUO3D\Developers\SDK\include
LIBS += -LC:\Users\Roman\Documents\DUO3D\Developers\SDK\windows\x86 \
                -lDUOLib \
		-lDense3DMT

SOURCES += main.cpp\
		mainwindow.cpp \
	stereocamera.cpp \
    image_output.cpp

HEADERS  += api_keys.h mainwindow.h stereocamera.h image_output.h \
	utils.h \
    api_keys.h

FORMS    += mainwindow.ui

