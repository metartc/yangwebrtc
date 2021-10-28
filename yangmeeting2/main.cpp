#include "mainwindow.h"
#include "yangutil/sys/YangSysMessageHandle.h"
#include "yangmeeting/YangMeetingFactory.h"
#include "src/video/yangvideothread.h"
#include <QApplication>
#include <QTextCodec>
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

#if (QT_VERSION <= QT_VERSION_CHECK(5,0,0))
#if _MSC_VER
    QTextCodec *codec = QTextCodec::codecForName("GBK");
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
#endif
    QTextCodec::setCodecForLocale(codec);
    QTextCodec::setCodecForCStrings(codec);
    QTextCodec::setCodecForTr(codec);
#else
    QTextCodec *codec = QTextCodec::codecForName("UTF-8");
    QTextCodec::setCodecForLocale(codec);
#endif


    MainWindow w;

    YangMeetingFactory mf;
    YangSysMessageHandle *sysmessage=mf.createSysMessageHandle(w.m_ini,&w);
    YangVideoThread videoThread;

    w.m_videoThread=&videoThread;
    w.m_videoMeeting=mf.getMeetingHandle(sysmessage);
    w.m_sysMessage=sysmessage;
    //w.m_videoMeeting->setMeetingMessage(w.m_tipThread);
    w.m_videoMeeting->setPlayvideoHandle(&videoThread);
    w.m_videoMeeting->init();
    w.m_videoContext.videoMeeting=w.m_videoMeeting;



    sysmessage->start();

    videoThread.m_meetingHandle=w.m_videoMeeting;
    videoThread.initPara(w.m_ini);
    w.initVideoThread();
    //videoThread.m_tip=w.m_tip;
    videoThread.start();

    w.setWindowIcon(QIcon(":/m7.ico"));

    w.show();
    return a.exec();
}
