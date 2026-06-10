TEMPLATE = app

TARGET = ppp

DESTDIR = tools/ppp

macx::CONFIG -= app_bundle

# postprocessing flags
MSVC_KEEP_DESTDIR = 1

win32 {
  QMAKE_CXXFLAGS += /TP

  DEFINES += WIN32 INVENTOR_EXPORTS __SGIOIV_BUILD_DLL
  DEFINES -= UNICODE
}
else:unix {
  DEFINES += NDEBUG
}

# include settings of required libraries
include( $(MEVIS_THIRDPARTY_LIB)/Configuration/MeVis_ThirdParty.pri )

# make sure there is no debug postfix on the target
TARGET = ppp

INCLUDEPATH += \
    lib/database/include \
    lib/interaction/include \
    lib/nodekits/include \
    lib/mevis/include

win32::CONFIG += console

SOURCES += \
    tools/ppp/ppp.cpp

