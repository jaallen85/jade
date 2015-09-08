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

# --------------------------------------------------------------------------------------------------

SOURCES += \
	source/DrawingItem.cpp \
	source/DrawingItemGroup.cpp \
	source/DrawingItemPoint.cpp \
	source/DrawingPathItem.cpp \
	source/DrawingShapeItems.cpp \
	source/DrawingTextItem.cpp \
	source/DrawingTwoPointItems.cpp \
	source/DrawingTypes.cpp \
	source/DrawingUndo.cpp \
	source/DrawingWidget.cpp \
	\
	source/MainToolBox.cpp \
	source/MainWindow.cpp \
	source/PropertiesWidget.cpp \
	source/main.cpp

HEADERS += \
	source/DrawingItem.h \
	source/DrawingItemGroup.h \
	source/DrawingItemPoint.h \
	source/DrawingPathItem.h \
	source/DrawingShapeItems.h \
	source/DrawingTextItem.h \
	source/DrawingTwoPointItems.h \
	source/DrawingTypes.h \
	source/DrawingUndo.h \
	source/DrawingWidget.h \
	\
	source/MainToolBox.h \
	source/MainWindow.h \
	source/PropertiesWidget.h

RESOURCES += icons/icons.qrc

RC_FILE = icons/icons.rc
OTHER_FILES += icons/icons.rc
