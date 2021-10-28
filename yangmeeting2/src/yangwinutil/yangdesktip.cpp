#include "yangdesktip.h"
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>
//#include <QApplication>
#include <QDebug>
#include "yangutil/yangtype.h"
//#include "../yangwinutil/yangcontext.h"
YangDeskTip::YangDeskTip(QWidget *parent) :QWidget(parent)
{
    m_Width=300;
    m_Height=200;
    setFixedSize(m_Width, m_Height);

    //setWindowFlags(Qt::FramelessWindowHint | Qt::WindowStaysOnTopHint);
    setWindowFlags(Qt::WindowMinMaxButtonsHint | Qt::FramelessWindowHint);
    m_widget = new QWidget();
    m_widget_1 = new QWidget();
    m_widget_2 = new QWidget();

    m_label = new QLabel(this);
   // m_close = new QPushButton();
    //m_close->setText("关闭");
   // m_close->setStyleSheet("QPushButton{border-style:none;}"
                          // "QPushButton:hover{color: #16EFCC}");
    ret=0;
    m_message = new QLabel();
    m_message->setAlignment(Qt::AlignCenter);

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->setAlignment(Qt::AlignRight);
   // h_layout->addWidget(m_close);
    h_layout->setMargin(0);
    m_widget_1->setLayout(h_layout);
    m_widget_1->setMaximumHeight(25);

    QHBoxLayout *h_layout1 = new QHBoxLayout();
    h_layout1->addWidget(m_message);
    h_layout1->setMargin(0);
    m_widget_2->setLayout(h_layout1);

    QVBoxLayout *v_layout = new QVBoxLayout();
    v_layout->addWidget(m_widget_1);
    v_layout->addWidget(m_widget_2);
    v_layout->setMargin(5);
    v_layout->setContentsMargins(5,5,5,5);
    m_widget->setLayout(v_layout);

    m_widget->setWindowOpacity(0.5);
    m_widget_2->setWindowOpacity(0.6);

    QVBoxLayout *layout = new QVBoxLayout;
    layout->setContentsMargins(0,0,5,5);
    layout->addWidget(m_widget);
    //m_message->setTextFormat(Qt::RichText);
    m_stringList=new QVector<QString>();
    setLayout(layout);
    fixHeight=0;
    Init();
    m_flashtime=3000;
    m_nWindowHeight=0;


}
YangDeskTip::~YangDeskTip(){
    yang_delete(m_stringList);
}
void YangDeskTip::Init()
{
   // if(m_nWindowHeight==0){
        //  QDesktopWidget* desktopWidget = QApplication::desktop();
        //获取可用桌面大小
        //  QGuiApplication::sc
        //   QRect rect = desktopWidget->availableGeometry();
        // mnDesktopHeight = desktopWidget->height();
    if(fixHeight==0){
        QRect rect = QGuiApplication::primaryScreen()->geometry();
        m_nWindowHeight = rect.height();
        m_point.setX(rect.width() - width());
        m_point.setY(rect.height() - height());
        fixHeight=m_point.y();

    }

}

void YangDeskTip::showMessage()
{
    // qDebug()<<"desktip is shwomessage....1..state=="<<m_threadState;
    //if(m_threadState) return;
    if(m_stringList->size()>0){
        QString tip_text("<p style='line-height:120%'>");
      if(m_stringList->size()>5) m_stringList->erase(m_stringList->begin());

         for (const QString &text : *m_stringList)
         {
             if (text.isEmpty())
                 continue;
             tip_text += text + "<br>";
         }
         tip_text += "</p>";
         m_message->setText(tip_text);
         m_stringList->clear();
    }
    show();
}
int YangDeskTip::flashMove(){
    showMessage();

    move(m_point.x(), m_nDesktopHeight);
    m_nDesktopHeight ++;
    ret=m_nDesktopHeight > m_nWindowHeight;
    if(ret){
        m_nDesktopHeight=0;
        clearText();
        hide();
    }
    return  ret;

}
//milli second
void YangDeskTip::setFlashTime(int flashtime){
    m_flashtime=flashtime;
}

int YangDeskTip::getWaitTime(){
    Init();
    if((m_nWindowHeight-fixHeight)>0){
        return (int)(m_flashtime/(m_nWindowHeight-fixHeight));
    }
    return 1;
}

void YangDeskTip::initShow(){
    m_nDesktopHeight=fixHeight;
}
void YangDeskTip::clearText(){
    m_message->setText("");
}
void YangDeskTip::setMessage(QString message)
{
    m_stringList->push_back(message);

   // m_message->setText(message);
 //   this->show();
}




