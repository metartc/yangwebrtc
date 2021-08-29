#ifndef YANGTHREAD_H
#define YANGTHREAD_H
#include <QThread>
#include <QVector>
#include <QWidget>
#include "yangswin.h"
#include "yangmeeting/yangmeetingtype.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "src/yangwinutil/yangtipthread.h"
#include "yangmeeting/YangMeetingHandle.h"
#include "src/yangplayer/YangPlayWidget.h"


struct YangShowWinType{
    int wid;
    int uid;

    YangYuvType captureFormat;
    long long timestamp;


    YangVideoBuffer *videoBuffer;
    YangPlayWidget *videoWindow;

};

class YangVideoThread : public QThread
{
    Q_OBJECT
public:
    YangVideoThread();
    virtual ~YangVideoThread();


    QVector<YangShowWinType*> m_showWins;
    YangMeetingHandle* m_meetingHandle;
    YangTipThread *m_tipThread;
    YangDeskTip *m_tip;
    YangYuvType m_captureFormat;
    int m_is16bit;
    int m_isStart;
    int m_isLoop;
    void initPara(YangMeetingContext *pini);
    void addWin(int wid,YangPlayWidget *pwd);
    void showWin(int wid,int puid);

    void clearRender();
  //  void removePreview();
    void delWd(int puid);
    void setShowMessage(QString str);

private:
    YangColor m_bgColor;
    YangColor m_textColor;
    int m_videoPlayNum;
    YangMeetingContext *m_ini;

    QVector<QString> m_messages;
    void render();
    void closeAll();
    int getVideoBufferIndex(int puid);
    int getShowVidesIndex(int pwid);
    int getShowVidesIndexByuid(int puid);

    YangVideoBuffer* getVideoBuffer(int puid);



private:
    virtual void run();
};

#endif // YANGTHREAD_H
