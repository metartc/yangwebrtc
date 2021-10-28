CONFIG -= qt

TEMPLATE = lib
CONFIG += staticlib

CONFIG += c++14

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
HOME_BASE=../
INCLUDEPATH += $$HOME_BASE/include
INCLUDEPATH += $$HOME_BASE/thirdparty/include
INCLUDEPATH += $$HOME_BASE/thirdparty/user_include
INCLUDEPATH += $$HOME_BASE/include_ext
INCLUDEPATH += $$HOME_BASE/thirdparty_ext/include
INCLUDEPATH += $$HOME_BASE/libyangmeeting2/src/include
unix{
     CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE/bin/lib_debug
    }else{
        DESTDIR += $$HOME_BASE/bin/lib_release
    }

}
win32{
    DEFINES +=_AMD64_
    INCLUDEPATH += $$HOME_BASE\thirdparty\include\win

  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }
    #if(contains(DEFINES,QT_WIN_MSC)){
    msvc{
        QMAKE_CFLAGS += /utf-8
        QMAKE_CXXFLAGS += /utf-8
        DEFINES +=HAVE_STRUCT_TIMESPEC
        DEFINES +=WIN32_LEAN_AND_MEAN
        INCLUDEPATH += $$HOME_BASE\thirdparty\include\win\include   #vc
    }
}
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/src/YangByteCount.cpp \
    src/src/YangCommandHandle.cpp \
    src/src/YangDataChannelConnection.cpp \
    src/src/YangMeetingAudioPlay.cpp \
    src/src/YangMeetingCaptureImpl.cpp \
    src/src/YangMeetingDecoder.cpp \
    src/src/YangMeetingEncoder.cpp \
    src/src/YangMeetingFactory.cpp \
    src/src/YangMeetingHandleImpl.cpp \
    src/src/YangMeetingMessageHandle.cpp \
    src/src/YangMeetingNet.cpp \
    src/src/YangMeetingPlay.cpp \
    src/src/YangMeetingPublish.cpp \
    src/src/YangMeetingType.cpp \
    src/src/YangPublish.cpp \
    src/src/YangReceive.cpp \
    src/src/YangUserData.cpp \
    src/src/YangUserDataBase.cpp \
    src/src/YangVrCapture.cpp

HEADERS += \
    src/include/yangmeeting/YangMeetingAudioPlay.h \
    src/include/yangmeeting/YangMeetingCapture.h \
    src/include/yangmeeting/YangMeetingDecoder.h \
    src/include/yangmeeting/YangMeetingEncoder.h \
    src/include/yangmeeting/YangMeetingFactory.h \
    src/include/yangmeeting/YangMeetingHandle.h \
    src/include/yangmeeting/YangMeetingMessageI.h \
    src/include/yangmeeting/YangMeetingNet.h \
    src/include/yangmeeting/YangMeetingPlay.h \
    src/include/yangmeeting/YangMeetingPublish.h \
    src/include/yangmeeting/YangPublish.h \
    src/include/yangmeeting/YangReceive.h \
    src/include/yangmeeting/YangVrCapture.h \
    src/include/yangmeeting/yangmeetingtype.h \
    src/include/yanguserdata/YangCommandHandle.h \
    src/include/yanguserdata/YangCommandId.h \
    src/include/yanguserdata/YangDataChannelConnection.h \
    src/include/yanguserdata/YangMessageHandleI.h \
    src/include/yanguserdata/YangUserData.h \
    src/include/yanguserdata/YangUserDataBase.h \
    src/src/YangByteCount.h \
    src/src/YangMeetingCaptureImpl.h \
    src/src/YangMeetingHandleImpl.h \
    src/src/YangMeetingMessageHandle.h
# Default rules for deployment.
unix {
    target.path = $$[QT_INSTALL_PLUGINS]/generic
}
!isEmpty(target.path): INSTALLS += target
