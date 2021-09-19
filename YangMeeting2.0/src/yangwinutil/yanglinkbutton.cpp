#include "yanglinkbutton.h"
#include <QDebug>
YangLinkButton::YangLinkButton(QWidget *parent) :
    QCommandLinkButton(parent)
{
    m_lbb=nullptr;
    connect(this,SIGNAL(clicked()), this, SLOT(clickbutton()));
}

YangLinkButton::~YangLinkButton()
{
    m_lbb=nullptr;
}

void  YangLinkButton::clickbutton(){
   // qDebug()<<"1............link button"<<"...lbb==="<<(m_lbb==nullptr?"is null":"is not null");
    if(m_lbb){
       //  qDebug()<<"2............link button"<<"...lbb===is not null";
        m_lbb->setIndex(m_index);
    }
}
