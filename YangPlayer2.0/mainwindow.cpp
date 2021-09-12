#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "yangutil/sys/YangSocket.h"
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_centerWdiget=new QWidget(this);
    m_vb=new QVBoxLayout();

    setCentralWidget(m_centerWdiget);
    m_centerWdiget->setLayout(m_vb);
    m_vb->setMargin(0);
    m_vb->setSpacing(0);
    m_videoWin=new YangPlayWidget(this);

    m_vb->addWidget(m_videoWin);
    m_hb1=new QHBoxLayout();
    m_hb2=new QHBoxLayout();
    m_hb1->addWidget(ui->m_l_url);
    m_hb1->addWidget(ui->m_url);
    m_hb1->addWidget(ui->m_b_play);
    //m_hb1->addWidget(ui->m_b_rtc);
    m_hb2->addWidget(m_videoWin);
    m_vb->addLayout(m_hb1);
    m_vb->addLayout(m_hb2);
    m_vb->setStretchFactor(m_hb1,1);
    m_vb->setStretchFactor(m_hb2,10);

    m_player= YangPlayerHandle::createPlayerHandle();

    YangSocket su;
    char s[128]={0};
    sprintf(s,"webrtc://%s:1985/live/livestream",su.getLocalInfo().c_str());
    ui->m_url->setText(s);

    //serverIp="192.168.3.26";
    //serverIp="192.168.43.172";
   // localServer=serverIp;
    localPort=8200;
    //app="live";
    //stream="livestream";


}

MainWindow::~MainWindow()
{
    delete ui;
    if(m_player){
        delete m_player;
        m_player=nullptr;
    }
}


void MainWindow::initVideoThread(YangRecordThread *prt){
    m_videoThread=prt;
    m_videoThread->m_video=m_videoWin;
      m_videoThread->initPara();

}

void MainWindow::on_m_b_play_clicked()
{
    if(m_player) {
        //initVideoThread();
        m_player->play(ui->m_url->text().toStdString(),localPort);
        m_videoThread->m_videoBuffer=m_player->getVideoBuffer();
    }
}


