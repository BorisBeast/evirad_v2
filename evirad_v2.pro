QT += core sql network websockets
QT -= gui

CONFIG += c++11

TARGET = evirad_v2
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    mainapp.cpp \
    processmanager.cpp

HEADERS += \
    mainapp.h \
    processmanager.h

target.path = /home/pi/Projekti
INSTALLS += target