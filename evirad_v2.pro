include(./sockets/sockets.pri)

QT += core sql network websockets
QT -= gui

CONFIG += c++11

TARGET = evirad_v2
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mainapp.cpp \
    processmanager.cpp \
    citac.cpp \
    kontroler.cpp \
    accesscontrol.cpp \
    socketmonitor.cpp \
    syncserver.cpp \
    syncclient.cpp \
    syncsocketserver.cpp

HEADERS += \
    mainapp.h \
    processmanager.h \
    citac.h \
    kontroler.h \
    accesscontrol.h \
    socketmonitor.h \
    syncserver.h \
    syncclient.h \
    syncsocketserver.h

target.path = /home/pi/Projekti
INSTALLS += target
