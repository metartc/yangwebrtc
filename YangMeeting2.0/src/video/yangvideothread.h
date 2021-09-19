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
    int32_t wid;
    int32_t uid;

    YangYuvType captureFormat;
    int64_t timestamp;


    YangVideoBuffer *videoBuffer;
    YangPlayWidget *videoWindow;

};

class YangVideoThread : public QThread,public YangVideoPlayHandle
{
    Q_OBJECT
public:
    YangVideoThread();
    virtual ~YangVideoThread();


    QVector<YangShowWinType*> m_showWins;
    YangMeetingHandle* m_meetingHandle;
    //YangTipThread *m_tipThread;
  //  YangDeskTip *m_tip;
    YangYuvType m_captureFormat;
    int32_t m_is16bit;
    int32_t m_isStart;
    int32_t m_isLoop;
    void initPara(YangMeetingContext *pini);
    void addWin(int wid,YangPlayWidget *pwd);
    void showWin(int wid,int puid);

    void clearRender();
  //  void removePreview();
    void delWd(int puid);
    void removePlayvideo(int32_t puid);
    //void setShowMessage(QString str);

   // void delMessage();

private:
    YangColor m_bgColor;
    YangColor m_textColor;
    int32_t m_videoPlayNum;
    YangMeetingContext *m_ini;

    QVector<QString> m_messages;
    void render();
    void closeAll();
    int32_t getVideoBufferIndex(int puid);
    int32_t getShowVidesIndex(int pwid);
    int32_t getShowVidesIndexByuid(int puid);

    YangVideoBuffer* getVideoBuffer(int puid);

    YangFrame m_frame;

private:
    virtual void run();
};

#endif // YANGTHREAD_H
