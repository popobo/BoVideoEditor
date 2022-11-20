#-------------------------------------------------
#
# Project created by QtCreator 2022-11-20T16:10:45
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = BoVideoEditor
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
        main.cpp \
    BoVideoUI.cpp \
    BoVideoWidget.cpp \
    BoVideoThread.cpp

HEADERS += \
    BoVideoUI.h \
    BoVideoWidget.h \
    BoVideoThread.h

FORMS += \
    BoVideoUI.ui

INCLUDEPATH += $$PWD/opencv/include

LIBS += $$PWD/opencv/lib/libopencv_world.so

LIBPATH += $$PWD/opencv/lib

QMAKE_RPATHDIR += $$PWD/opencv/lib

#MOC_DIR = $$PWD/build/temp/moc
#RCC_DIR = $$PWD/build/temp/rcc
#UI_DIR = $$PWD/build/temp/ui
#OBJECTS_DIR = $$PWD/build/temp/obj

CONFIG(debug, debug|release){
    DESTDIR = $$PWD/build/debug
}else{
    DESTDIR = $$PWD/build/release
}
