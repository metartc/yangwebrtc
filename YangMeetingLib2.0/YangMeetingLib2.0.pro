CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
   HOME_BASE=../
unix{

    INCLUDEPATH += $$HOME_BASE/include
    INCLUDEPATH += $$HOME_BASE/thirdparty/include
    INCLUDEPATH += $$HOME_BASE/YangMeetingLib2.0/src
     CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{
        DESTDIR += $$HOME_BASE/bin/lib_release
    }

}
win32{

    DEFINES +=_AMD64_

    INCLUDEPATH += $$HOME_BASE\include
    INCLUDEPATH += $$HOME_BASE\thirdparty\include
    INCLUDEPATH += $$HOME_BASE\thirdparty\include\win
       INCLUDEPATH += $$HOME_BASE\YangMeetingLib2.0\src
  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }
}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/yangmeeting/src/YangByteCount.cpp \
    src/yangmeeting/src/YangMeetingAudioPlay.cpp \
    src/yangmeeting/src/YangMeetingCaptureImpl.cpp \
    src/yangmeeting/src/YangMeetingDecoder.cpp \
    src/yangmeeting/src/YangMeetingEncoder.cpp \
    src/yangmeeting/src/YangMeetingFactory.cpp \
    src/yangmeeting/src/YangMeetingHandleImpl.cpp \
    src/yangmeeting/src/YangMeetingMessageHandle.cpp \
    src/yangmeeting/src/YangMeetingNet.cpp \
    src/yangmeeting/src/YangMeetingPlay.cpp \
    src/yangmeeting/src/YangMeetingPublish.cpp \
    src/yangmeeting/src/YangPublish.cpp \
    src/yangmeeting/src/YangReceive.cpp \
    src/yangmeeting/src/YangVrCapture.cpp \
    src/yanguserdata/src/YangCommandHandle.cpp \
    src/yanguserdata/src/YangDataChannelConnection.cpp \
    src/yanguserdata/src/YangUserData.cpp \
    src/yanguserdata/src/YangUserDataBase.cpp

HEADERS += \
    src/yangmeeting/include/YangMeetingAudioPlay.h \
    src/yangmeeting/include/YangMeetingCapture.h \
    src/yangmeeting/include/YangMeetingDecoder.h \
    src/yangmeeting/include/YangMeetingEncoder.h \
    src/yangmeeting/include/YangMeetingNet.h \
    src/yangmeeting/include/YangMeetingPlay.h \
    src/yangmeeting/include/YangMeetingPublish.h \
    src/yangmeeting/include/YangPublish.h \
    src/yangmeeting/include/YangReceive.h \
    src/yangmeeting/include/YangVrCapture.h \
    src/yangmeeting/src/YangByteCount.h \
    src/yangmeeting/src/YangMeetingCaptureImpl.h \
    src/yangmeeting/src/YangMeetingHandleImpl.h \
    src/yangmeeting/src/YangMeetingMessageHandle.h \
    src/yanguserdata/include/YangCommandHandle.h \
    src/yanguserdata/include/YangCommandId.h \
    src/yanguserdata/include/YangDataChannelConnection.h \
    src/yanguserdata/include/YangMessageHandleI.h \
    src/yanguserdata/include/YangUserData.h \
    src/yanguserdata/include/YangUserDataBase.h

# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
