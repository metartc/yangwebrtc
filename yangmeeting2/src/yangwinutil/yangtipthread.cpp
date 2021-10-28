#include "yangtipthread.h"
#include <QDebug>
#include <yangutil/yang_unistd.h>
YangTipThread::YangTipThread()
{
    m_threadState=0;
    m_tip=nullptr;//new YangDeskTip();
   // m_loop=0;
    m_timer=new YangTimer();
    m_timer->setTask(this);
    m_timer->setTaskId(1);
    m_waitTime=0;
    m_flashtime=3000;
    //m_timer->setTimelen(10);
    //m_tip->Init();
}
YangTipThread::~YangTipThread()
{
    // if(m_tip) delete m_tip;
  //  m_loop=0;
    if(m_timer->m_isStart){
        m_timer->stop();
        while(m_timer->m_isStart) yang_usleep(500);
    }

    m_tip=nullptr;
}

void YangTipThread::doTask(int taskId){
    if(m_tip==NULL) return;

    if(m_tip->flashMove()) {

       stopMessage();

    }
}
void YangTipThread::startMessage(){
    qDebug()<<"startMessage==="<<m_threadState;
    if( m_threadState==1) return;
    if(m_waitTime==0){
        m_tip->setFlashTime(m_flashtime);
        m_waitTime=m_tip->getWaitTime();
        m_timer->setTimelen(m_waitTime);
        qDebug()<<"m_waitTime======================="<<m_waitTime;
    }
     m_threadState=1;
     yang_sleep(2);
     if(m_tip) m_tip->initShow();
    m_timer->start();

}
void YangTipThread::stopMessage(){
    m_timer->stop();
    m_threadState=0;
}
int YangTipThread::getMessageState(){
    return  m_threadState;
}


void YangTipThread::setShowMessage(QString str){
    if(m_tip) m_tip->setMessage(str);
    //  this->start();
}
