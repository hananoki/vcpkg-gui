QT       += core gui concurrent
win32:QT += winextras

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

CONFIG += lrelease

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


PRECOMPILED_HEADER = src/pch.h

INCLUDEPATH += src src/ui ../Qtlib/src

CONFIG( debug, debug|release ) {
    # debug
    win32:LIBS += -L../Qtlib/debug
} else {
    # release
    win32:LIBS += -L../Qtlib/build/release
}


LIBS += Qtlib.lib


RESOURCES += \
  src/res.qrc

TRANSLATIONS = \
  ../translations/main_ja.ts

#RC_ICONS = src/image/logo_400x400.ico

SOURCES += \
    src/App.cpp \
    src/AppHelper.cpp \
    src/Config.cpp \
    src/Language.cpp \
    src/VCPKGProcess.cpp \
    src/ui/UIAvailablePackages.cpp \
    src/ui/UIInstallPackage.cpp \
    src/ui/UIMainStatusBar.cpp \
    src/ui/UIMainCaption.cpp \
    src/ui/UIMainWindow.cpp \
    src/ui/UITripletList.cpp

HEADERS += \
    src/App.h \
    src/AppHelper.h \
    src/Config.h \
    src/VCPKGProcess.h \
    src/Language.h \
    src/ui/UIAvailablePackages.h \
    src/ui/UIInstallPackage.h \
    src/ui/UIMainStatusBar.h \
    src/ui/UIMainCaption.h \
    src/ui/UIMainWindow.h \
    src/ui/UITripletList.h

FORMS += \
    src/ui/UIAvailablePackages.ui \
    src/ui/UIInstallPackage.ui \
    src/ui/UIMainStatusBar.ui \
    src/ui/UIMainCaption.ui \
    src/ui/UIMainWindow.ui \
    src/ui/UITripletList.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

