#include "yangdesktip.h"
#include <QHBoxLayout>
#include <QGuiApplication>
#include <QScreen>
//#include <QApplication>
#include <QDebug>
#include "yangutil/yangtype.h"
#include "src/yangwinutil/yangcontext.h"
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
    m_close = new QPushButton();
    m_close->setText("关闭");
    m_close->setStyleSheet("QPushButton{border-style:none;}"
                           "QPushButton:hover{color: #16EFCC}");

    m_message = new QLabel();
    m_message->setAlignment(Qt::AlignCenter);

    QHBoxLayout *h_layout = new QHBoxLayout();
    h_layout->setAlignment(Qt::AlignRight);
    h_layout->addWidget(m_close);
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

    setLayout(layout);

    Init();

    m_nWindowHeight=0;


}
YangDeskTip::~YangDeskTip(){
    // YANG_DELETE(m_stringList);
}
void YangDeskTip::Init()
{
    // connect(m_pShowTimer, SIGNAL(timeout()), this, SLOT(onMove()));
    // connect(m_pStayTimer, SIGNAL(timeout()), this, SLOT(onStay()));
    // connect(m_pCloseTimer, SIGNAL(timeout()), this, SLOT(onClose()));
    //connect(m_close,&QPushButton::clicked,this,&YangDeskTip::onClickClose);
}
void YangDeskTip::showMessage(QString message)
{
    setMessage(message);
    showMessage();
    //this->start();
}
//void YangDeskTip::onShowMessage(){
 //   m_message->setText(m_text);
  //  showMessage();
   // this->start();
//}
void YangDeskTip::showMessageStay(QString message)
{
    setMessage(message);
    showMessage();
}
void YangDeskTip::showMessage()
{
    // qDebug()<<"desktip is shwomessage....1..state=="<<m_threadState;
    //if(m_threadState) return;
    show();
    if(m_nWindowHeight==0){
        //  QDesktopWidget* desktopWidget = QApplication::desktop();
        //获取可用桌面大小
        //  QGuiApplication::sc
        //   QRect rect = desktopWidget->availableGeometry();
        // mnDesktopHeight = desktopWidget->height();
        QRect rect = QGuiApplication::primaryScreen()->geometry();
        m_nWindowHeight = rect.height();
        m_point.setX(rect.width() - width());
        m_point.setY(rect.height() - height());
    }
    m_nDesktopHeight=m_point.y();
    move(m_point.x(), m_nDesktopHeight);
    //  this->start();
    //m_pShowTimer->start(5);
    // qDebug()<<"desktip is shwomessage....";
}

//void YangDeskTip::setTitle(QString title, QIcon icon)
//{
//  m_title->setIcon(icon);
//  m_label->setText(title);
//}

void YangDeskTip::setMessage(QString message)
{
    /**  QString tip_text("<p style='line-height:120%'>");
    if(m_stringList->size()>5) m_stringList->erase(m_stringList->begin());
    m_stringList->push_back(message);
       for (const QString &text : *m_stringList)
       {
           if (text.isEmpty())
               continue;
           tip_text += text + "<br>";
       }
       tip_text += "</p>";
       m_message->setText(tip_text);**/
    m_message->setText(message);
 //   this->show();
}




//void YangDeskTip::onClose()
//{
  //  this->hide();
//}

