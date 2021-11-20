#include "recordmainwindow.h"
#include "ui_recordmainwindow.h"
#include <yangpush/YangPushCommon.h>
#include <yangutil/sys/YangLog.h>
#include <yangutil/sys/YangFile.h>
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangIni.h>
#include <yangutil/sys/YangString.h>
#include <yangpush/YangPushFactory.h>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopWidget>

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
    m_videoType=Yang_VideoSrc_Camera;//Yang_Screen;
    m_isStartpush=0;

    m_win0=NULL;
    m_isVr=0;


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
    m_screenInternal=33;
    m_hasAudio=true;


}

RecordMainWindow::~RecordMainWindow()
{
    closeAll();

}

void RecordMainWindow::closeEvent( QCloseEvent * event ){

    closeAll();
    exit(0);
}

void RecordMainWindow::screenEvent(char* str)
{
    vector<string> commands=yang_split(str,',');
    if(commands.size() != 4)
        return ;

    string type = commands[0];
    string directionStr = commands[1];
    YangScreenKeyEvent ev;
    ev.direction=directionStr;
    ev.event=type;
    if(directionStr=="key"){
        ev.key=atoi(commands[3].c_str());
        ev.x=0;
        ev.y=0;

    }else{
        int xxx = m_screenWidth * atof(commands[2].c_str());
        int yyy = m_screenHeight * atof(commands[3].c_str());
       ev.x=xxx;
       ev.y=yyy;

      }
       if(type=="wheel") ev.wheel=atoi(directionStr.c_str());
       remoteScreenWinEvent(&ev);

}
void RecordMainWindow::receiveSysMessage(YangSysMessage *mss, int32_t err){
    switch (mss->messageId) {
    case YangM_Push_Publish_Start:
        {
            if(err){
                ui->m_b_rec->setText("开始");
                m_isStartpush=!m_isStartpush;
                QMessageBox::about(NULL,  "error","push error("+QString::number(err)+")!");
            }
        }
        break;
   case YangM_Push_Publish_Stop:
        break;
    case YangM_Push_StartScreenCapture:
        m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
          qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_rt->m_videoBuffer<<"....ret===="<<err;
         break;
    case YangM_Push_StartVideoCapture:
    {
        m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
        qDebug()<<"message==="<<m_message<<"..prevideobuffer==="<<m_rt->m_videoBuffer<<"....ret===="<<err;
         break;
     }
    case YangM_Push_SwitchToCamera:
         m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
         break;
    case YangM_Push_SwitchToScreen:
        m_rt->m_videoBuffer=m_pushfactory.getPreVideoBuffer(m_message);
         break;
    }


}
void RecordMainWindow::initVideoThread(YangRecordThread *prt){
    m_rt=prt;
    m_rt->m_video=m_win0;
    m_rt->initPara(m_ini);

}
void RecordMainWindow::closeAll(){

    if(m_ini){

        m_rt->stopAll();


        m_rt=NULL;

        yang_delete(m_message);
        yang_delete(m_ini);
        delete ui;
    }
}

void RecordMainWindow::initPreview(){
    if(m_videoType==Yang_VideoSrc_Screen) {
        ui->m_c_screen->setCheckState(Qt::Checked);
        ui->m_c_screen->setEnabled(false);
        yang_post_message(YangM_Push_StartScreenCapture,0,NULL);
    }else{
        yang_post_message(YangM_Push_StartVideoCapture,0,NULL);
    }


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
    m_screenWidth=desk->screenGeometry().width();
    m_screenHeight=desk->screenGeometry().height();
    m_screenInfo.width=m_screenWidth;
    m_screenInfo.height=m_screenHeight;
    m_screenInfo.outWidth=m_screenWidth;
    m_screenInfo.outHeight=m_screenHeight;

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
        qDebug()<<"url========="<<ui->m_url->text().toLatin1().data();
        m_url=ui->m_url->text().toLatin1().data();
        yang_post_message(YangM_Push_Publish_Start,0,NULL,(void*)m_url.c_str());
    }else{
        ui->m_b_rec->setText("开始");
       yang_post_message(YangM_Push_Publish_Stop,0,NULL);
              m_isStartpush=!m_isStartpush;
    }

}



void RecordMainWindow::switchToCamera(){
      yang_post_message(YangM_Push_SwitchToCamera,0,NULL);
}
void RecordMainWindow::switchToScreen(){
     yang_post_message(YangM_Push_SwitchToScreen,0,NULL);
    ui->m_c_screen->setCheckState(Qt::Checked);
    ui->m_c_screen->setEnabled(false);

}
//remote screen control
void RecordMainWindow::remoteScreenWinEvent(YangScreenKeyEvent* ev)
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
