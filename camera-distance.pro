#-------------------------------------------------
#
# Project created by QtCreator 2016-03-07T14:39:04
#
#-------------------------------------------------

QT       += core gui widgets opengl

TARGET = camera-distance
TEMPLATE = app

CONFIG += c++11

win32{
 OPENCV_INC = C:\opencv\build\include
 OPENCV_LIBS = -LC:\opencv\build\x86\vc11\lib \
		-lopencv_core2412 \
		-lopencv_highgui2412 \
		-lopencv_imgproc2412 \
		-lopencv_features2d2412 \
		-lopencv_calib3d2412

 DUO_INC = C:\Users\Roman\Documents\DUO3D\Developers\SDK\include
 DUO_LIBS = -LC:\Users\Roman\Documents\DUO3D\Developers\SDK\windows\x86 -lDUOLib
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
 DUO_LIBS = -L$$PWD/SDK/osx/x64/ -lDUO
}

# OpenCV dependency
INCLUDEPATH += $${OPENCV_INC}
LIBS += $${OPENCV_LIBS}
# DUO3D dependency
INCLUDEPATH += $${DUO_INC}
LIBS += $${DUO_LIBS} \
		-lDense3DMT

SOURCES += main.cpp\
		mainwindow.cpp \
	stereocamera.cpp \
    glwidget.cpp

HEADERS  += api_keys.h mainwindow.h stereocamera.h \
	utils.h \
	api_keys.h \
    glwidget.h \
    distance_point.h \
    thread_safe_queue.h

FORMS    += mainwindow.ui

