#LIBS += -lusb-1.0
INCLUDEPATH += $$PWD
DEPENDPATH += $$PWD

SOURCES += \
    $$PWD/portserial.cpp \
    $$PWD/udpsocket.cpp
HEADERS += \
    $$PWD/portserial.h \
    $$PWD/socket.h \
    $$PWD/udpsocket.h
win32 {
    SOURCES += \
        $$PWD/portserial_com.cpp
}
unix {
    SOURCES += \
        $$PWD/portserial_tty.cpp
}
