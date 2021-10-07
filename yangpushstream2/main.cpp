#include "recordmainwindow.h"
#if defined(_MSC_VER) && (_MSC_VER >= 1600)
#pragma execution_character_set("utf-8")
#endif
#include <QApplication>
#include "src/video/yangrecordthread.h"
#include "yangutil/sys/YangSysMessageHandle.h"
#include "yangrecliving/YangRecordUtilFactory.h"
#include <QTextCodec>
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


    RecordMainWindow w;
    YangRecordThread videoThread;




    w.initVideoThread(&videoThread);
    videoThread.start();



    w.show();
    return a.exec();
}
