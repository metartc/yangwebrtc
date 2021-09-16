QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
unix{
    HOME_BASE=../
    INCLUDEPATH += $$HOME_BASE/include
    CONFIG(debug, debug|release) {
        #LIBS += -L$$HOME_BASE/bin/lib_debug
        LIBS += -L$$HOME_BASE/bin/lib_debug -L$$HOME_BASE/thirdparty/lib
        DESTDIR += $$HOME_BASE/bin/app_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_release
        DESTDIR += $$HOME_BASE/bin/app_release
    }
 LIBS += -lYangAVLib2  -lpthread -lasound -ldl  -lssl2 -lcrypto2  -lsrtp2
}
win32{
     HOME_BASE=../
    INCLUDEPATH += $$HOME_BASE\include
    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_win_debug
        DESTDIR += $$HOME_BASE/bin/app_win_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_win_release
        DESTDIR += $$HOME_BASE/bin/app_win_release
    }
    LIBS +=  -L$$HOME_BASE/thirdparty/lib/win
     LIBS += -lYangAVLib2 -lws2_32 -lwinmm -ldmoguids -lole32 -lStrmiids  -lsrtp2  -lssl -lcrypto -lCrypt32 -lws2_32
}

SOURCES += \
    main.cpp \
    recordmainwindow.cpp \
    src/video/YangPlayWidget.cpp \
    src/video/yangrecordthread.cpp \
    src/video/yangrecordvideowin.cpp


HEADERS += \
    recordmainwindow.h \
    src/video/YangPlayWidget.h \
    src/video/yangrecordthread.h \
    src/video/yangrecordvideowin.h \
    src/video/yangvideotype.h

FORMS += \
    recordmainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
