#include "yangvideothread.h"
#include <QDebug>
#include <QMapIterator>
#include "yangmeeting/yangmeetingtype.h"
#include "YangVideoWinHandleH.h"
#include "yangutil/yangtype.h"



YangVideoThread::YangVideoThread()
{
    m_isLoop=0;
    m_bgColor={0,0,0};
    m_textColor={0,0,255};
    m_captureFormat=YangI420;//YangYv12;
    m_is16bit=1;
    m_videoPlayNum=5;
    m_isStart=0;
    memset(&m_ys,0,sizeof(m_ys));
}

YangVideoThread::~YangVideoThread(){
   // g_videoThread=nullptr;
   // m_meetingHandle=nullptr;

    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
            QThread::msleep(1);
        }
    }
    closeAll();
}



void YangVideoThread::closeAll(){

    clearRender();


}

void YangVideoThread::clearRender(){

}




void YangVideoThread::reInitWin(int32_t pwid,int32_t phei){
    if(m_ys.videoWindow){
        m_ys.width=pwid;
        m_ys.height=phei;
         m_ys.videoWindow->initBg(pwid,phei);
    }
}

void YangVideoThread::render(){
        if(!m_ys.videoWindow){
                    YangWinPlayFactroy pf;
                    m_ys.videoWindow=pf.createWinPlay();
                    m_ys.videoWindow->init(m_ys.winId);
                    m_ys.videoWindow->initBg(m_ys.width,m_ys.height);
                    m_ys.init=0;

        }

        if(m_ys.videoBuffer){

            if(!m_ys.init&&m_ys.videoBuffer->m_width>0){            
                m_ys.videoWindow->initVideo(m_ys.videoBuffer->m_width,m_ys.videoBuffer->m_height,m_captureFormat);        
                m_ys.init=1;
            }

            if(m_ys.videoBuffer->m_size>0)  {

                uint8_t* tmp=m_ys.videoBuffer->getVideoRef(&m_frame);

                if(tmp) m_ys.videoWindow->render(tmp);
                         tmp=nullptr;
                //if(m_ys.videoBuffer&&m_ys.videoBuffer->m_size>m_videoPlayNum) m_ys.videoBuffer->resetIndex();
            }
        }else{
            if(m_ys.videoWindow) m_ys.videoWindow->renderBg(&m_bgColor);

        }

}

void YangVideoThread::run(){

    m_isStart=1;
    m_isLoop=1;

    while(m_isLoop){
        QThread::msleep(20);
        render();
    }
    closeAll();
    m_isStart=0;
}
