#ifndef YANGVIDEOS_H
#define YANGVIDEOS_H

#include "yangutil/buffer/YangVideoBuffer.h"
#include<QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "yangvideowin.h"
#include "src/video/yangswin.h"
#include "../yangwinutil/yangqInterfaces.h"
#include "src/video/yangselectuservideo.h"
#include "src/yangwinutil/yangvideocontext.h"
#include "meetingtable.h"
class YangVideos:public QObject,public YangMenuHandleI
{
    Q_OBJECT
public:
    YangVideos(YangVideoContext* pcontext);
       ~YangVideos();
    QMainWindow *m_win;
    QVBoxLayout *m_vb;
    YangSelectUserVideo *m_selectUser;
    int32_t cind;
    void initAll();
    void initVideos();
    void handleWin(int pind);
    void handleMenu(int ptype);
    void unplayVideo(int puid);
    YangVideoWin *m_videos[4];//*video1,*video2,*video3,*video4;
    int32_t m_videoType=0,m_pre_videoType=0;

    MeetingTable* m_table;
signals:
   void QuitWindowEvent();
private:
    int32_t m_camSt;
    YangVideoContext* m_context;

};

#endif // YANGVIDEOS_H
