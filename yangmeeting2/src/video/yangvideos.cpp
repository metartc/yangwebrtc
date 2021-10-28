#include "yangvideos.h"
#include <QMenu>
#include<QApplication>
#include <QMessageBox>
#include <QDebug>
#include "../yangwinutil/yangvideocontext.h"

#include "yangutil/sys/YangSysMessageI.h"

YangVideos::YangVideos(YangVideoContext* pcontext)
{
    m_context=pcontext;
    m_win=nullptr;
    m_vb=nullptr;

    for(int i=0;i<4;i++){
        m_videos[i]=nullptr;
    }
    m_selectUser=nullptr;
    cind=0;
    m_camSt=1;
    m_table=nullptr;

}
YangVideos::~YangVideos()
{
    m_win=nullptr;
    m_vb=nullptr;
    m_table=nullptr;
    yang_delete(m_selectUser);
    for(int i=0;i<4;i++){
        yang_delete(m_videos[i]);//=nullptr;
    }
}
void YangVideos::initVideos(){
    for(int i=0;i<4;i++){
        m_videos[i]=new YangVideoWin(m_context,m_context->mainWin);
        m_videos[i]->sid=i+1;
      }

}

void YangVideos::handleWin(int pind){
    if(cind==pind) return;
   if(cind>0) m_videos[cind-1]->clearVideo();
    m_context->videoThread->clearRender();
    if(m_vb){
        m_vb->removeWidget(m_videos[cind-1]);
        if(cind>0) m_videos[cind-1]->hide();
        m_vb->addWidget(m_videos[pind-1]);
        m_videos[pind-1]->resize(m_context->mainWin->width(),m_context->mainWin->height());
        m_videos[pind-1]->init();
        m_videos[pind-1]->show();
        m_videos[pind-1]->addShowVideo();
    }
    cind=pind;
}
void YangVideos::unplayVideo(int puid){

    if(cind>0) m_videos[cind-1]->removeVideo(puid);
}

void YangVideos::handleMenu(int ptype){
    //qDebug()<<"video_type=="<<m_videoType<<"..ptype=="<<ptype;
    if(ptype<5&&m_videoType==ptype) return;
    if(ptype<5) {
        m_pre_videoType=m_videoType;
        handleWin(ptype);
        m_videoType=ptype;
    }else{
        int32_t userId=m_context->context->user.userId;
        switch (ptype)
        {
        case 5:
            yang_post_message(YangM_Sys_Pubaudio,userId,NULL);
            break;
        case 6:
            yang_post_message(YangM_Sys_UnPubaudio,userId,NULL);
            break;
        case 7:
            yang_post_message(YangM_Sys_Pubvideo,userId,NULL);
            break;
        case 8:
            yang_post_message(YangM_Sys_UnPubvideo,userId,NULL);
            break;
        case 9:
            yang_post_message(YangM_Sys_DisconnectServer,userId,NULL);
            if(m_table) m_table->show();

            break;
        case 10:

            emit QuitWindowEvent();
            break;
        case 11:
             m_camSt++;
            if(m_camSt==4) m_camSt=1;
            m_context->videoMeeting->change(m_camSt);
            break;
        case 12:
            yang_post_message(YangM_Sys_Setvr,userId,NULL);
            break;
        case 13:
            yang_post_message(YangM_Sys_UnSetvr,userId,NULL);
            break;
        default:
            break;
        }
    }

}


void YangVideos::initAll(){
    if(m_selectUser==nullptr) m_selectUser=new YangSelectUserVideo(m_win);
    for(int i=0;i<4;i++){
        m_videos[i]->m_selectUser=m_selectUser;
        m_videos[i]->init();
    }
}



