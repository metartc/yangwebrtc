#include "yangtipthread.h"
#include <QDebug>
YangTipThread::YangTipThread()
{
    m_threadState=0;
    m_tip=nullptr;//new YangDeskTip();
    m_loop=0;
    //m_tip->Init();
}
YangTipThread::~YangTipThread()
{
  // if(m_tip) delete m_tip;
    m_loop=0;
    if(m_threadState){
        while(m_threadState){
            usleep(500);
        }
    }
    m_tip=nullptr;
}

void YangTipThread::run(){
     m_tip->showMessage();
    m_threadState=1;

    sleep(2);

    m_loop=1;
    while(m_loop){
        usleep(1000);
        m_tip->move(m_tip->m_point.x(), m_tip->m_nDesktopHeight);
        m_tip->m_nDesktopHeight ++;
        if(m_tip->m_nDesktopHeight > m_tip->m_nWindowHeight) m_loop=0;
    }

    m_loop=0;
    m_threadState=0;
}
void YangTipThread::setShowMessage(QString str){
    if(m_threadState) return;

    if(m_tip) m_tip->setMessage(str);
  //  this->start();
}
