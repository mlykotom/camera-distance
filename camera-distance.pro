#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T14:39:04
#
#-------------------------------------------------

QT       += core gui widgets

TARGET = camera-distance
TEMPLATE = app

#CONFIG += console
#CONFIG -= app_bundle

#DEFINES += SHOW_DEBUG_OUTPUT

QMAKE_CXXFLAGS += -Woverloaded-virtual

# OpenCV dependency
INCLUDEPATH += /usr/local/include/
LIBS += -L/usr/local/lib \
		-lopencv_core \
		-lopencv_highgui \
		-lopencv_imgproc \
		-lopencv_features2d \
		-lopencv_calib3d

# DUO3D dependency
INCLUDEPATH += $$PWD/SDK/include
LIBS += -L$$PWD/SDK/osx/x64/ \
		-lDUO \
		-lDense3DMT

SOURCES += main.cpp\
		mainwindow.cpp \
	stereocamera.cpp

HEADERS  += api_keys.h mainwindow.h stereocamera.h image_output.h \
    utils.h

FORMS    += mainwindow.ui

