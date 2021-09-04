#include "yangvideothread.h"
#include <QDebug>
#include <QMapIterator>
#include "yangmeeting/yangmeetingtype.h"
#include "YangVideoWinHandleH.h"
#include "yangutil/yangtype.h"
YangVideoThread* g_videoThread=NULL;

void yang_clearRender(){
    if(g_videoThread) g_videoThread->clearRender();
}
void yang_add_winvideo(int pwid,YangPlayWidget * pwin){
    if(g_videoThread) g_videoThread->addWin(pwid,pwin);
}
void yang_add_winshowvideo(int pwid,int puid){
    if(g_videoThread) g_videoThread->showWin(pwid,puid);
}
YangVideoThread::YangVideoThread()
{
    m_isLoop=0;
    m_bgColor={0,0,0};
    m_textColor={0,0,255};
    m_captureFormat=YangYv12;
    m_is16bit=1;
    m_videoPlayNum=5;
    m_isStart=0;
    //m_tipThread=nullptr;
   // m_tip=nullptr;
    g_videoThread=this;
    m_meetingHandle=nullptr;
}

YangVideoThread::~YangVideoThread(){
    g_videoThread=nullptr;
    m_meetingHandle=nullptr;
    //if(m_tipThread) m_tipThread->stop();
   // m_tip=nullptr;

    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
            QThread::msleep(1);
        }
    }
    closeAll();
}

void YangVideoThread::initPara(YangMeetingContext *pini){
    m_ini=pini;
    if(pini->video.videoCaptureFormat==YangI420){
        m_captureFormat=YangI420;
        m_is16bit=0;
    }
    if(pini->video.videoDecHwType==0) {
        m_captureFormat=YangYv12;//YangI420;//YangYv12;
        // qDebug()<<"*****************YangYv12*************add video uid==";
    }
    m_videoPlayNum=pini->video.videoPlayCacheNum;
}

void YangVideoThread::closeAll(){
   // if(m_tipThread) delete m_tipThread;
   // m_tipThread=nullptr;
    clearRender();


}

void YangVideoThread::removePlayvideo(int32_t puid){
    delWd(puid);
}

void YangVideoThread::clearRender(){
    for(int i=0;i<m_showWins.size();i++){
        YangShowWinType *ys1=m_showWins[i];
        if(ys1){
            ys1->videoBuffer=nullptr;
            ys1->videoWindow=nullptr;
            delete m_showWins[i];
            m_showWins[i]=nullptr;
        }


        ys1=NULL;
    }
    m_showWins.clear();
}
void YangVideoThread::delWd(int puid){
    int32_t ind=getShowVidesIndexByuid(puid);
    if(ind>-1){
        YangShowWinType *ys=m_showWins[ind];
        ys->uid=-1;
       ys->videoBuffer=nullptr;
       // ys->videoWindow=nullptr;

        ys=NULL;
    }
}

void YangVideoThread::showWin(int pwid,int puid){

    delWd(puid);
    int32_t ind=getShowVidesIndex(pwid);
    if(ind>-1) {

        m_showWins[ind]->videoBuffer=nullptr;
        if(m_ini->user.userId==puid){
            m_showWins[ind]->captureFormat=YangI420;//:m_captureFormat
             m_showWins[ind]->videoBuffer=m_meetingHandle->getPreviewVideo();

        }else{
            m_showWins[ind]->captureFormat=YangI420;//m_captureFormat;//:m_captureFormat         
             m_showWins[ind]->videoBuffer=getVideoBuffer(puid);
           //  qDebug()<<"...videbufer====="<< m_showWins[ind]->videoBuffer;
        }
        m_showWins[ind]->uid=puid;

    }

}



void YangVideoThread::addWin(int wid,YangPlayWidget *pwd){
    qDebug()<<"add wid===="<<wid;
    int32_t ind1=getShowVidesIndex(wid);
    if(ind1==-1){
        m_showWins.push_back(new YangShowWinType{wid,-1,m_captureFormat,0,NULL,pwd});

    }

}




YangVideoBuffer* YangVideoThread::getVideoBuffer(int puid){
    int32_t ind=-1;
    std::vector<YangVideoBuffer*> *vecs=m_ini->videoBuffers;
    if(vecs==NULL) return NULL;
    for (int i=0;i<(int)vecs->size();i++ ) {
        YangVideoBuffer* yb=vecs->at(i);
        if(yb->m_uid==puid) {
            ind=i;
        }
        yb=NULL;
    }
    vecs=NULL;
    if(ind>-1) return m_ini->videoBuffers->at(ind);
    return NULL;
}

int YangVideoThread::getShowVidesIndexByuid(int puid){
    for (int i=0;i<(int)m_showWins.size();i++ ) {
        if(m_showWins[i]->uid==puid) return i;
    }
    return -1;
}

int YangVideoThread::getShowVidesIndex(int wid){

    for (int i=0;i<(int)m_showWins.size();i++ ) {
        if(m_showWins[i]->wid==wid) return i;
    }
    return -1;
}


void YangVideoThread::render(){
   // qDebug()<<"size==="<<m_showWins.size();
    for(int i=0;i<m_showWins.size();i++){
        YangShowWinType *ys=m_showWins[i];
        if(ys->uid==-1){
            ys->videoBuffer=NULL;
        }else{
            if(ys->uid>-1&&ys->videoBuffer==NULL){
                ys->videoBuffer=getVideoBuffer(ys->uid);
            }
        }
        if(ys->videoBuffer&&ys->videoBuffer->m_size>0){
                uint8_t* p=ys->videoBuffer->getVideoIn(&ys->timestamp);
                if(p) ys->videoWindow->playVideo(p,ys->videoBuffer->m_width,ys->videoBuffer->m_height);
                if(ys->videoBuffer->m_size>m_videoPlayNum) ys->videoBuffer->resetIndex();
                p=NULL;
        }

        ys=NULL;
    }//end for
}

void YangVideoThread::run(){
    m_isStart=1;
    m_isLoop=1;

    // int32_t interval=1000*1000/50;
    while(m_isLoop){
        QThread::msleep(20);
          render();
    }
    closeAll();
    m_isStart=0;
}
