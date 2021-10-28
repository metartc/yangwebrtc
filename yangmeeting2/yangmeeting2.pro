QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT +=network
QT +=core xml
CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE += qtIcon.rc


HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/libyangmeeting2/src/include
LIBS += -lyangrtcmeeting2 -lyangrtc2  -lavutil -lavcodec
#-lyangrtcext2
unix{

    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_debug -L$$HOME_BASE/thirdparty/lib
        DESTDIR += $$HOME_BASE/bin/app_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_release -L$$HOME_BASE/thirdparty/lib
        DESTDIR += $$HOME_BASE/bin/app_release
    }
 LIBS +=  -lpthread -lasound -ldl  -lssl2 -lcrypto2 -lsrtp2
}
win32{
    DEFINES +=_AMD64_

    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_win_debug
        DESTDIR += $$HOME_BASE/bin/app_win_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_win_release
        DESTDIR += $$HOME_BASE/bin/app_win_release
    }
    LIBS +=  -lksuser -lwinmm  -lStrmiids -lsrtp2
    YANG_LIB= -L$$HOME_BASE/thirdparty/lib/win -lssl  -lcrypto
    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
        QMAKE_LFLAGS    += /ignore:4099
        DEFINES +=HAVE_STRUCT_TIMESPEC
        DEFINES +=WIN32_LEAN_AND_MEAN
        INCLUDEPATH += $$HOME_BASE\thirdparty\include\win\include   #vc pthreadVC2
        YANG_LIB=  -L$$HOME_BASE/thirdparty/lib/win/msvc -luser32  -lavrt -lAdvapi32  -llibcrypto -llibssl
    }

    LIBS +=  $$YANG_LIB
    LIBS +=  -lCrypt32 -lws2_32 -lole32 -lpthread
}
#RC_ICONS = m2.png
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += \
    logindialog.cpp \
    main.cpp \
    mainwindow.cpp \
    meetingtable.cpp \
    src/login/yangUserInfo.cpp \
    src/login/yangmeeting.cpp \
    src/login/yangmeetings.cpp \
    src/video/yangselectuservideo.cpp \
    src/video/yangswin.cpp \
    src/video/yangvideos.cpp \
    src/video/yangvideothread.cpp \
    src/video/yangvideowin.cpp \
    src/yangplayer/YangPlayWidget.cpp \
    src/yangwinutil/yangdesktip.cpp \
    src/yangwinutil/yanghttputil.cpp \
    src/yangwinutil/yanglinkbutton.cpp \
    src/yangwinutil/yangtipthread.cpp \
    src/yangwinutil/yangvideocontext.cpp

HEADERS += \
    logindialog.h \
    mainwindow.h \
    meetingtable.h \
    src/login/yangUserInfo.h \
    src/login/yangmeeting.h \
    src/login/yangmeetings.h \
    src/video/YangVideoWinHandleH.h \
    src/video/yangselectuservideo.h \
    src/video/yangswin.h \
    src/video/yangvideos.h \
    src/video/yangvideothread.h \
    src/video/yangvideotype.h \
    src/video/yangvideowin.h \
    src/yangplayer/YangPlayWidget.h \
    src/yangwinutil/yangdesktip.h \
    src/yangwinutil/yanghttputil.h \
    src/yangwinutil/yanglinkbutton.h \
    src/yangwinutil/yangqInterfaces.h \
    src/yangwinutil/yangtipthread.h \
    src/yangwinutil/yangvideocontext.h

FORMS += \
    logindialog.ui \
    mainwindow.ui \
    meetingtable.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target


DISTFILES += \
    qtIcon.rc
