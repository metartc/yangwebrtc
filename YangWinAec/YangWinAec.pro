CONFIG -= qt

TEMPLATE = lib
DEFINES += YANGWINAEC_LIBRARY

CONFIG += c++11

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
#DEFINES +=  _WIN32_WINNT= 0x0A00
# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.

 HOME_BASE=..\
    INCLUDEPATH += $$HOME_BASE\include
    INCLUDEPATH += $$HOME_BASE\thirdparty\include
    INCLUDEPATH += $$HOME_BASE\thirdparty\include\win

  CONFIG(debug, debug|release) {
        DESTDIR += $$HOME_BASE\bin\lib_win_debug
    }else{
        DESTDIR += $$HOME_BASE\bin\lib_win_release
    }
 LIBS +=  -lwinmm -ldmoguids -lole32 -lStrmiids
SOURCES +=  \
    src/YangWinAudioDevice.cpp \
    src/yangwinaec.cpp

HEADERS +=  \   \
    src/YangWinAecI.h \
    src/YangWinAec_global.h \
    src/yangwinaec.h
    src/yangwinaec.h

# Default rules for deployment.
unix {
    target.path = /usr/lib
}
!isEmpty(target.path): INSTALLS += target
