#ifndef YANGVIDEOS_H
#define YANGVIDEOS_H
//#include "../../mainwindow.h"
#include "yangutil/buffer/YangVideoBuffer.h"
#include<QMainWindow>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include "yangvideowin.h"
#include "src/video/yangswin.h"
#include "../yangwinutil/yangqInterfaces.h"
#include "src/video/yangselectuservideo.h"
class YangVideos:public QObject,public YangMenuHandleI
{
    Q_OBJECT
public:
    YangVideos();
       ~YangVideos();
    QMainWindow *m_win;
    QVBoxLayout *m_vb;
    YangSelectUserVideo *m_selectUser;
    int cind;
    void initAll();
    void initVideos();
    void handleWin(int pind);
    void handleMenu(int ptype);
    void unplayVideo(int puid);
    YangVideoWin *m_videos[4];//*video1,*video2,*video3,*video4;
    int m_videoType=0,m_pre_videoType=0;
signals:
   void QuitWindowEvent();
private:
    int m_camSt;

};

#endif // YANGVIDEOS_H
