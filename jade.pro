TEMPLATE = app

TARGET = jade
DESTDIR = bin

QT += core gui
greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += release embed_manifest_dll c++11
CONFIG -= debug

DEFINES += QT_DEPRECATED_WARNINGS

INCLUDEPATH += source source/drawing

SOURCES += \
	source/drawing/DrawingCanvas.cpp \
	source/drawing/DrawingTypes.cpp \
	source/drawing/DrawingWidget.cpp \
	source/drawing/DrawingWindow.cpp \
	\
	source/MainWindow.cpp \
	source/main.cpp

HEADERS += \
	source/drawing/DrawingCanvas.h \
	source/drawing/DrawingTypes.h \
	source/drawing/DrawingWidget.h \
	source/drawing/DrawingWindow.h \
	\
	source/MainWindow.h

RESOURCES += icons/icons.qrc

RC_FILE = icons/icons.rc
OTHER_FILES += icons/icons.rc

!win32:MOC_DIR = release
!win32:OBJECTS_DIR = release
!win32:RCC_DIR = release

# Default rules for deployment
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
