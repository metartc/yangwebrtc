#include "recordmainwindow.h"
#include "ui_recordmainwindow.h"

//#include "yangrecliving/YangLivingtype.h"
#include <QDebug>
#include "yangutil/sys/YangLog.h"
#include "yangutil/sys/YangFile.h"
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangIni.h>
#include <QMessageBox>
#include <QDesktopWidget>
//#include "yangrecliving/YangLivingMessageHandle.h"
RecordMainWindow::RecordMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecordMainWindow)
{

    ui->setupUi(this);

    m_ini=new YangContext();
    m_ini->init((char*)"yang_config.ini");

    init();
    yang_setLogLevle(m_ini->sys.logLevel);
    yang_setLogFile(m_ini->sys.hasLogFile);


    //m_showev->event=1;

    m_isStartpush=0;

    m_win0=NULL;
    m_isVr=0;
    m_rec=YangPushHandle::createPushHandle(m_ini,this);
    m_rec->setScreenVideoInfo(&m_screenInfo);
    m_rec->init();

    m_hb0=new QHBoxLayout();
    ui->vdMain->setLayout(m_hb0);
    m_win0=new YangPlayWidget(this);
    m_hb0->addWidget(m_win0);
    m_hb0->setMargin(0);
    m_hb0->setSpacing(0);

    char s[128]={0};
    sprintf(s,"webrtc://%s:1985/live/livestream",yang_getLocalInfo().c_str());
    ui->m_url->setText(s);

    m_isDrawmouse=true; //screen draw mouse

}

RecordMainWindow::~RecordMainWindow()
{
    closeAll();

}

void RecordMainWindow::closeEvent( QCloseEvent * event ){

    closeAll();
    exit(0);
}
void RecordMainWindow::initVideoThread(YangRecordThread *prt){
    m_rt=prt;
    m_rt->initPara(m_ini);
    initPreview();
}
void RecordMainWindow::closeAll(){
    if(m_isStartpush){
        m_rec->disconnect();
        m_isStartpush=!m_isStartpush;
    }
    if(m_rec){
        if(m_isStartpush){
            m_rec->disconnect();
            m_isStartpush=!m_isStartpush;
        }
        m_rt->stopAll();
        m_rec=nullptr;

        m_rt=NULL;

        yang_closeLogFile();
        yang_delete(m_ini);
        delete ui;
    }
}

void RecordMainWindow::initPreview(){
    m_rt->m_video=m_win0;
    m_rt->m_videoBuffer=m_rec->getPreVideoBuffer();
}


void RecordMainWindow::init() {
    m_ini->audio.usingMono=0;
    m_ini->audio.sample=48000;
    m_ini->audio.channel=2;
    m_ini->audio.hasAec=0;
    m_ini->audio.audioCacheNum=8;
    m_ini->audio.audioCacheSize=8;
    m_ini->audio.audioPlayCacheNum=8;

    m_ini->video.videoCacheNum=10;
    m_ini->video.evideoCacheNum=10;
    m_ini->video.videoPlayCacheNum=10;


    m_ini->audio.audioEncoderType=3;
    m_ini->sys.rtcLocalPort=17000;
    m_ini->enc.enc_threads=4;

    memcpy(&m_screenInfo,&m_ini->video,sizeof(YangVideoInfo));
    QDesktopWidget* desk=QApplication::desktop();
    int width=desk->screenGeometry().width();
    int height=desk->screenGeometry().height();
    m_screenInfo.width=width;
    m_screenInfo.height=height;
    m_screenInfo.outWidth=width;
    m_screenInfo.outHeight=height;

}

void RecordMainWindow::success(){

}
void RecordMainWindow::failure(int32_t errcode){
    //on_m_b_play_clicked();
    QMessageBox::aboutQt(NULL,  "push error("+QString::number(errcode)+")!");

}

void RecordMainWindow::on_m_b_rec_clicked()
{
    if(!m_isStartpush){

        ui->m_b_rec->setText("停止");
        m_isStartpush=!m_isStartpush;
        if(m_rec) {

            int err=m_rec->publish(ui->m_url->text().toStdString(),yang_getLocalInfo().c_str(),m_ini->sys.rtcLocalPort);
            if(err){
                ui->m_b_rec->setText("开始");
                if(m_rec) m_rec->disconnect();
                m_isStartpush=!m_isStartpush;
                QMessageBox::about(NULL,  "error","push error("+QString::number(err)+")!");
            }
        }

    }else{
        ui->m_b_rec->setText("开始");
        if(m_rec) m_rec->disconnect();
              m_isStartpush=!m_isStartpush;
    }

}



void RecordMainWindow::switchToCamera(){
    m_rec->switchToCamera();
    m_rt->m_videoBuffer=m_rec->getPreVideoBuffer();
    qDebug()<<"width==="<<m_rt->m_videoBuffer->m_width<<"..height=="<<m_rt->m_videoBuffer->m_height;
}
void RecordMainWindow::switchToScreen(){
    m_rec->switchToScreen();
    m_rec->setScreenInterval(100);
    m_rec->setDrawmouse(m_isDrawmouse);
    m_rt->m_videoBuffer=m_rec->getPreVideoBuffer();
    qDebug()<<"width==="<<m_rt->m_videoBuffer->m_width<<"..height=="<<m_rt->m_videoBuffer->m_height;
}
//remote screen control
void RecordMainWindow::remoteScreenWinEvent(YangScreenEvent* ev)
{
#ifdef _WIN32
    if(ev->event == "mousePress" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftBDown();
    }
    else if(ev->event == "mousePress" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBDown();
    }
    if(ev->event == "mouseRelease" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftBUp();
    }
    else if(ev->event == "mouseRelease" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBUp();
    }
    if(ev->event == "mouseDouble" && ev->direction == "left")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.leftbDClick();
    }
    else if(ev->event == "mouseDouble" && ev->direction == "right")
    {
        m_winMouse.moveTo(ev->x,ev->y);
        m_winMouse.rightBDbClick();
    }
    else if(ev->event == "mouseMove")
    {
        m_winMouse.moveTo(ev->x,ev->y);
    }else if(ev->event == "keyPressEvent")
    {
        keybd_event(ev->key, 0, 0, 0);

    }
    else if( ev->event == "keyReleaseEvent")
    {
        keybd_event(ev->key, 0, KEYEVENTF_KEYUP, 0);

    }else if(ev->event=="wheel"){
        m_winMouse.middleBRoll(ev->x,ev->y,ev->wheel);
    }
#endif
}


void RecordMainWindow::on_m_c_vr_clicked()
{
    m_isVr=ui->m_c_vr->checkState()==Qt::Checked?1:0;
    if(m_isVr){
        //if(m_rec) m_rec->startVr(m_ini->bgFilename);
        // yang_post_message(YangM_Rec_Setvr,0,NULL);
    }else{
        //if(m_rec) m_rec->stopVr();
        // yang_post_message(YangM_Rec_UnSetvr,0,NULL);
    }
}

void RecordMainWindow::on_m_c_screen_clicked()
{
    if(ui->m_c_screen->checkState()==Qt::Checked){
        switchToScreen();
    }else{
        switchToCamera();
    }
}
