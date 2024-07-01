QT       += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
win32:LIBS += -lkernel32 -luser32
# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    DataEncryption.cpp \
    devicecontroller.cpp \
    main.cpp \
    mainwindow.cpp \
 \   # mbedtl_src.cpp
    myDatagram.cpp

HEADERS += \
    DataEncryption.h \
    devicecontroller.h \
    mainwindow.h \
    mbedtl_def.h \
    myDatagram.h \
    transit_def.h

FORMS += \
    mainwindow.ui


# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
