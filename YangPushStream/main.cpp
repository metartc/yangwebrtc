#include "recordmainwindow.h"

#include <QApplication>
#include "src/video/yangrecordthread.h"
#include "yangutil/sys/YangSysMessageHandle.h"
#include "yangrecliving/YangRecordUtilFactory.h"
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    RecordMainWindow w;


 //   w.m_recMessage=livingMessage;
    YangRecordThread videoThread;




    w.initVideoThread(&videoThread);
    videoThread.start();



    w.show();
    return a.exec();
}
