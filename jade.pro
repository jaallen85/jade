TEMPLATE = app

win32:TARGET = Jade
!win32:TARGET = jade

DESTDIR = bin
INCLUDEPATH += source

CONFIG += release warn_on embed_manifest_dll c++11 qt
CONFIG -= debug
QT += widgets

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

INCLUDEPATH += ../libjade/include
LIBS += ../libjade/lib/jade.lib

# --------------------------------------------------------------------------------------------------

SOURCES += \
    source/ItemPropertiesWidget.cpp \
    source/MainWindow.cpp \
    source/main.cpp

HEADERS += \
    source/ItemPropertiesWidget.h \
    source/MainWindow.h
