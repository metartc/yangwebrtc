#include "yangswin.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEngine>
#include "yangutil/yangtype.h"
#include "../yangwinutil/yangcontext.h"
#include "yangutil/sys/YangLog.h"
#include "YangVideoWinHandleH.h"
YangVideoShowWin::~YangVideoShowWin()
{
    m_selectUser=nullptr;
    m_videoHandle=nullptr;
 }
YangVideoShowWin::YangVideoShowWin(QWidget *parent) : YangPlayWidget(parent)
{
   // ++wids;
#ifdef _WIN32
    wid=rand();
 #else
    wid=random();
 #endif
    m_selectUser=nullptr;
    swId=1;
    vuid=-1;
    m_videoHandle=nullptr;
}
int YangVideoShowWin::getVWinId(){

    return wid;
}
void YangVideoShowWin::success(){
    yang_add_winshowvideo(wid,vuid);
}
void YangVideoShowWin::failure(int errcode){

    YangVideoContext::mainWin->showMessage("播放视频失败！Error:"+QString(yang_getErrorStr(errcode)));
}
QPaintEngine* YangVideoShowWin::paintEngine()const{
    return nullptr;
}
//int YangVideoShowWin::wids=0;

void YangVideoShowWin::mousePressEvent(QMouseEvent *qe){
    if(YangVideoContext::mainWin->m_videoMeeting->m_state!=0) return;
    if(qe->button() == Qt::LeftButton){

        if(m_selectUser){
            m_selectUser->m_uid=&vuid;
            m_selectUser->m_message=this;
            if(!m_selectUser->m_videoHandle) m_selectUser->m_videoHandle=m_videoHandle;
            m_selectUser->init();
            m_selectUser->wid=wid;
            m_selectUser->show();
            m_selectUser->activateWindow();           
            m_selectUser->m_widget=this;
            m_selectUser->move(qe->globalX(),qe->globalY());
        }

    }
}
void YangVideoShowWin::onVideoClick(){

}
