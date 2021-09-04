#ifndef YANGTHREAD_H
#define YANGTHREAD_H
#include <QThread>
#include <QVector>
#include <QWidget>
#include "yangswin.h"
#include "yangmeeting/yangmeetingtype.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include "yangmeeting/YangMeetingHandle.h"

struct YangShowWinType{
    void* winId;
    //int32_t uid;
    int32_t init;
    int64_t timestamp;
    int32_t width;
    int32_t height;
    char hasName;
    YangVideoBuffer *videoBuffer;
    YangWinPlay *videoWindow;
    char* videoName;
};

class YangVideoThread : public QThread
{
    Q_OBJECT
public:
    YangVideoThread();
    virtual ~YangVideoThread();

   // QVector<YangThreadWinType*> m_videos;
   // QVector<YangShowWinType*> m_showWins;
   // YangMeetingHandle* m_meetingHandle;

    YangYuvType m_captureFormat;
    YangShowWinType m_ys;
    int32_t m_is16bit;
    int32_t m_isStart;
    int32_t m_isLoop;
   // void initPara(YangMeetingContext *pini);
    void reInitWin(int32_t pwid,int32_t phei);


    void clearRender();
  //  void removePreview();
    void delWd(int32_t puid);
    void setShowMessage(QString str);

private:
    YangColor m_bgColor;
    YangColor m_textColor;
    int32_t m_videoPlayNum;
    YangMeetingContext *m_ini;

    QVector<QString> m_messages;
    void render();
    void closeAll();

   // YangVideoBuffer* getVideoBuffer(int32_t puid);
  //  void initPlaySdl(YangShowWinType *pwin);


private:
    virtual void run();
};

#endif // YANGTHREAD_H
