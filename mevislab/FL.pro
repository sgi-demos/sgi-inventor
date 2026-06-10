TEMPLATE = lib

TARGET = FL

CONFIG += staticlib exceptions thread
CONFIG += freetype

# FL uses fontconfig on Linux
unix:!macx:CONFIG += fontconfig

DESTDIR    = libFL
DLLDESTDIR = ../../../lib

win32::DEFINES += WIN32 INVENTOR_EXPORTS __SGIOIV_BUILD_DLL
win32::DEFINES -= UNICODE

debug::DEFINES += DEBUG

# include settings of required libraries
include( $(MEVIS_THIRDPARTY_LIB)/Configuration/MeVis_ThirdParty.pri )

# libFL    
INCLUDEPATH += \
    libFL/ang \
    libFL/src

# libFL    
HEADERS += \
    libFL/ang/fl.h \
    libFL/ang/flendian.h \
    libFL/ang/flfreetype.h \
    libFL/src/flclient.h

SOURCES += \
    libFL/ang/fl.c \
    libFL/ang/flclient.c \
    libFL/ang/flfreetype.c

