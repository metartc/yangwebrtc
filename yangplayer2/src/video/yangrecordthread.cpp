#include "yangrecordthread.h"
#include <QDebug>
#include <QMapIterator>
#include <yangutil/sys/YangLog.h>
YangRecordThread::YangRecordThread()
{

    m_isLoop=0;
    m_video=nullptr;
    m_syn=nullptr;


    m_sid=1;
    showType=1;

    m_isStart=0;
}

YangRecordThread::~YangRecordThread(){
    // m_ys=nullptr;
    // m_vb=NULL;
    m_video=nullptr;
    m_syn=nullptr;
    //  m_vb=NULL;

    stopAll();
}
void YangRecordThread::stopAll(){
    if(m_isLoop){
        m_isLoop=0;
        while (m_isStart) {
            QThread::msleep(1);
        }
    }
    closeAll();
}
void YangRecordThread::initPara(){

}
void YangRecordThread::closeAll(){
    //clearRender();
}

int adlen=0;

void YangRecordThread::render(){
    if(m_syn){
         if(m_syn->getVideoSize()){
             adlen=0;
             if(adlen>3)           yang_trace("\nempty queue.............%d...",adlen);

        }else
            adlen++;

        uint8_t* t_vb=m_syn->getVideoRef(&m_frame);

        if(t_vb&&m_video){
            m_video->playVideo(t_vb,m_syn->m_width,m_syn->m_height);
        }

        t_vb=NULL;
    }
}

void YangRecordThread::run(){
    // init();

    m_isLoop=1;
    m_isStart=1;


    while(m_isLoop){

        QThread::msleep(20);
        render();
    }
    m_isStart=0;
    // closeAll();
}
