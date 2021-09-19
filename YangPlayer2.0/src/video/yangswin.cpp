#include "yangswin.h"
#include <QDebug>
#include <QMouseEvent>
#include <QPaintEngine>
#include "yangutil/yangtype.h"

#include "yangutil/sys/YangLog.h"
#include "YangVideoWinHandleH.h"
YangVideoShowWin::~YangVideoShowWin()
{
   // m_selectUser=nullptr;
    //m_videoHandle=nullptr;
    //YANG_DELETE(m_ypainter);
    // timer = nullptr;
    //m_vb=NULL;
}
YangVideoShowWin::YangVideoShowWin(QWidget *parent) : QWidget(parent)
{

    // setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    //  this->setParent(parent);
    setAttribute(Qt::WA_PaintOnScreen);
    // setAttribute(Qt::WA_OpaquePaintEvent); //WA_NoSystemBackground
    setAttribute(Qt::WA_NoSystemBackground);
    //   setAttribute(Qt::WA_DontCreateNativeAncestors, true);

    setAttribute(Qt::WA_NativeWindow, true);//Qt::WA_DontCreateNativeAncestors
    //  setAttribute(Qt::AA_NativeWindows, true);


    setMouseTracking(true);
    setUpdatesEnabled(false);
   // ++wids;
   // wid=random();
  //  qDebug()<<"wid=================="<<wid;
  //  YangVideoContext::yangThread->m_showWins.push_back(new YangShowWin);
   // YangShowWin *ys=YangVideoContext::yangThread->m_showWins.back();
  //  ys->wid=wid;
   // ys->winId=(void*)winId();
 //   ys->videoWindow=nullptr;
    //m_selectUser=nullptr;
    //swId=1;
    //vuid=-1;
   // m_videoHandle=nullptr;


}

QPaintEngine* YangVideoShowWin::paintEngine()const{
    return nullptr;
}
//int YangVideoShowWin::wids=0;



