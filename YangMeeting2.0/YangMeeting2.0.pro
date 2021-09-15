QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets
QT +=network
QT +=core xml
CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

RC_FILE += qtIcon.rc

msvc {
    QMAKE_CFLAGS += /utf-8
    QMAKE_CXXFLAGS += /utf-8
}
unix{
    HOME_BASE=../
    INCLUDEPATH += $$HOME_BASE/include
    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_debug -L$$HOME_BASE/thirdparty/lib
        DESTDIR += $$HOME_BASE/bin/app_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_release -L$$HOME_BASE/thirdparty/lib
        DESTDIR += $$HOME_BASE/bin/app_release
    }
 LIBS += -lYangMeetingLib2 -lYangAVLib2  -lpthread -lasound -ldl  -lssl2 -lcrypto2 -lsrtp2
}
win32{

    #DEFINES += __WIN32__
    DEFINES +=_AMD64_
    HOME_BASE=..\
    INCLUDEPATH += $$HOME_BASE\include
    CONFIG(debug, debug|release) {
        LIBS += -L$$HOME_BASE/bin/lib_win_debug
        DESTDIR += $$HOME_BASE/bin/app_win_debug
    }else{
        LIBS += -L$$HOME_BASE/bin/lib_win_release
        DESTDIR += $$HOME_BASE/bin/app_win_release
    }
    LIBS +=  -L$$HOME_BASE/thirdparty/lib/win
    #LIBS +=
    LIBS +=  -lYangMeetingLib2 -lYangAVLib2  -lwinmm -ldmoguids -lole32 -lStrmiids -lsrtp2  -lssl -lcrypto -lCrypt32 -lws2_32
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
    src/yangwinutil/yangcontext.cpp \
    src/yangwinutil/yangdesktip.cpp \
    src/yangwinutil/yanghttputil.cpp \
    src/yangwinutil/yanglinkbutton.cpp \
    src/yangwinutil/yangtipthread.cpp

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
    src/yangwinutil/yangcontext.h \
    src/yangwinutil/yangdesktip.h \
    src/yangwinutil/yanghttputil.h \
    src/yangwinutil/yanglinkbutton.h \
    src/yangwinutil/yangqInterfaces.h \
    src/yangwinutil/yangtipthread.h

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
