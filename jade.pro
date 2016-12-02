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
	source/AboutDialog.cpp \
	source/DiagramReader.cpp \
	source/DiagramUndo.cpp \
    source/DiagramWidget.cpp \
    source/DiagramWriter.cpp \
    source/DynamicPropertiesWidget.cpp \
	source/ElectricItems.cpp \
	source/HelperWidgets.cpp \
	source/LogicItems.cpp \
	source/MainWindow.cpp \
	source/PreferencesDialog.cpp \
	source/main.cpp

HEADERS += \
	source/AboutDialog.h \
	source/DiagramReader.h \
	source/DiagramUndo.h \
    source/DiagramWidget.h \
    source/DiagramWriter.h \
    source/DynamicPropertiesWidget.h \
	source/ElectricItems.h \
	source/HelperWidgets.h \
	source/LogicItems.h \
	source/MainWindow.h \
	source/PreferencesDialog.h

RESOURCES += icons/icons.qrc

RC_FILE = icons/icons.rc
OTHER_FILES += icons/icons.rc
