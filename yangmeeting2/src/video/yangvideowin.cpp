#include "yangvideowin.h"
#include "yangutil/yangtype.h"
#include <QMenu>
#include <QAction>
#include <QDebug>
#include "YangVideoWinHandleH.h"
#include "../yangwinutil/yangvideocontext.h"
YangVideoWin::YangVideoWin(YangVideoContext* pcontext,QWidget *parent):QWidget(parent)
{
    m_context=pcontext;
    m_hb=new QHBoxLayout();
    m_hb->setMargin(0);
    m_hb->setSpacing(1);
    setLayout(m_hb);
    setAutoFillBackground(true);
    m_selectUser=nullptr;
    vwin1=nullptr;
    vwin2=nullptr;
    vwin3=nullptr;
    vwin4=nullptr;

    m_vb1=nullptr;
    m_vb2=nullptr;
    m_widget1=NULL;
    m_widget2=NULL;
    sid=0;
}

YangVideoWin::~YangVideoWin()
{
    m_selectUser=nullptr;
    yang_delete(vwin1);
    yang_delete(vwin2);
    yang_delete(vwin3);
    yang_delete(vwin4);

    yang_delete(m_vb1);
    yang_delete(m_vb2);
    yang_delete(m_hb);
    yang_delete(m_widget1);
    yang_delete(m_widget2);
}

void YangVideoWin::addShowVideo(){
    yang_clearRender();
    if(vwin1){ yang_add_winvideo(vwin1->getVWinId(),vwin1);}
    if(vwin2){ yang_add_winvideo(vwin2->getVWinId(),vwin2);}
    if(vwin3){ yang_add_winvideo(vwin3->getVWinId(),vwin3);}
    if(vwin4){ yang_add_winvideo(vwin4->getVWinId(),vwin4);}
}

void YangVideoWin::removeVideo(int puid){
    // qDebug()<<"win1uid==%d"<<vwin1->vuid<<"....removeVideo=="<<puid;
    if(vwin1&&vwin1->vuid==puid) vwin1->vuid=-1;
    if(vwin2&&vwin2->vuid==puid) vwin2->vuid=-1;
    if(vwin3&&vwin3->vuid==puid) vwin3->vuid=-1;
    if(vwin4&&vwin4->vuid==puid) vwin4->vuid=-1;
}
void YangVideoWin::clearVideo(){
     if(vwin1) vwin1->vuid=-1;
    if(vwin2) vwin2->vuid=-1;
    if(vwin3) vwin3->vuid=-1;
    if(vwin4) vwin4->vuid=-1;
}
void YangVideoWin::initSelect(){
    if(m_selectUser){
        if(vwin1&&vwin1->m_selectUser==nullptr)  vwin1->m_selectUser=m_selectUser;
        if(vwin2&&vwin2->m_selectUser==nullptr)  vwin2->m_selectUser=m_selectUser;
        if(vwin3&&vwin3->m_selectUser==nullptr)  vwin3->m_selectUser=m_selectUser;
        if(vwin4&&vwin4->m_selectUser==nullptr)  vwin4->m_selectUser=m_selectUser;
        //m_selectUser->init();
    }
   // if(vwin4) { vwin4->swId=4;YangVideoContext::yangThread->add(vwin4->wid,vwin4);}

    if(vwin1){ vwin1->swId=1;vwin1->m_videoHandle=this;}
    if(vwin2){  vwin2->swId=2;vwin2->m_videoHandle=this;}
    if(vwin3) { vwin3->swId=3;vwin3->m_videoHandle=this;}
    if(vwin4) { vwin4->swId=4;vwin4->m_videoHandle=this;}


}

void YangVideoWin::init(){

    init(sid);
}

void YangVideoWin::init(int pind){
    if(pind==1) init1();
    if(pind==2) init2();
    if(pind==3) init3();
    if(pind==4) init4();
}
void YangVideoWin::init1(){
    if(vwin1==nullptr) {
        vwin1=new YangVideoShowWin(m_context,this);
        vwin1->m_selectUser=m_selectUser;
        m_hb->addWidget(vwin1);
        vwin1->resize(m_context->mainWin->width(),m_context->mainWin->height());
        // vwin1->resize(this->width(),this->height());
    }

    initSelect();
    int32_t wid=m_context->mainWin->width();
    int32_t hei=m_context->mainWin->height();
    vwin1->resize(wid,hei);


}
void YangVideoWin::init2(){
    if(vwin1==nullptr) {
        vwin1=new YangVideoShowWin(m_context,this);
        m_hb->addWidget(vwin1);
    }
    if(vwin2==nullptr){
        vwin2=new YangVideoShowWin(m_context,this);
        m_hb->addWidget(vwin2);
    }

    initSelect();
    int32_t wid=m_context->mainWin->width()/2;
    int32_t hei=m_context->mainWin->height();
    vwin1->resize(wid,hei);
    vwin2->resize(wid,hei);
}
void YangVideoWin::init3(){
    if(vwin1==nullptr) {
        vwin1=new YangVideoShowWin(m_context,this);
        m_hb->addWidget(vwin1);
    }
    if(!m_widget1) m_widget1=new QWidget();
    if(m_vb1==nullptr){

        m_vb1=new QVBoxLayout();
        m_vb1->setMargin(0);
        m_vb1->setSpacing(1);

        if(vwin2==nullptr){
            vwin2=new YangVideoShowWin(m_context,this);
            m_vb1->addWidget(vwin2);
        }
        if(vwin3==nullptr){
            vwin3=new YangVideoShowWin(m_context,this);
            m_vb1->addWidget(vwin3);
        }
        m_widget1->setLayout(m_vb1);
        m_hb->addWidget(m_widget1);
    }


    m_hb->setStretch(0,1);
    m_hb->setStretch(1,1);
    initSelect();
    int32_t wid=m_context->mainWin->width();
    int32_t hei=m_context->mainWin->height();
    vwin1->resize(wid/2,hei);
    m_widget1->resize(wid/2,hei);
}
void YangVideoWin::init4(){
    if(!m_widget1) m_widget1=new QWidget(this);
    if(!m_widget2) m_widget2=new QWidget(this);
    if(m_vb1==nullptr){
        m_vb1=new QVBoxLayout();
        m_vb1->setMargin(0);
        m_vb1->setSpacing(1);

        if(vwin1==nullptr){
            vwin1=new YangVideoShowWin(m_context,this);
            m_vb1->addWidget(vwin1);
        }

        if(vwin3==nullptr){
            vwin3=new YangVideoShowWin(m_context,this);
            m_vb1->addWidget(vwin3);
        }

        m_widget1->setLayout(m_vb1);
    }
    if(m_vb2==nullptr){
        m_vb2=new QVBoxLayout();
        m_vb2->setMargin(0);
        m_vb2->setSpacing(1);
        if(vwin2==nullptr){
            vwin2=new YangVideoShowWin(m_context,this);
            m_vb2->addWidget(vwin2);
        }
        if(vwin4==nullptr){
            vwin4=new YangVideoShowWin(m_context,this);
            m_vb2->addWidget(vwin4);
        }
        m_widget2->setLayout(m_vb2);
        m_hb->addWidget(m_widget1);
        m_hb->addWidget(m_widget2);
    }

    m_hb->setStretch(0,1);
    m_hb->setStretch(1,1);
    initSelect();
    int32_t wid=m_context->mainWin->width();
    int32_t hei=m_context->mainWin->height();
    m_widget1->resize(wid/2,hei);
    m_widget2->resize(wid/2,hei);

}

