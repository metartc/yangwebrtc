#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDesktopWidget>
#include "src/yangwinutil/yangcontext.h"
#include "yangmeeting/YangMeetingFactory.h"
#include <sys/stat.h>
#include "yangutil/sys/YangLog.h"
#include "yangutil/sys/YangSysMessageI.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    m_exitState=0;
    //this->setWindowIcon()
    //m_ld=new LoginDialog(this);
    //this->showFullScreen();
    setWindowFlags(Qt::FramelessWindowHint);
    setAttribute(Qt::WA_StyledBackground);
    this->showMaximized();
    m_isInit=0;
    m_ini=new YangMeetingContext();
    YangMeetingFactory mf;
    memset(m_ini, 0, sizeof(m_ini));
    mf.createIni("yang_config.ini",m_ini);
    yang_setLogLevle(m_ini->sys.logLevel);
    yang_setLogFile(m_ini->sys.hasLogFile);
    initConf();

    YangVideoContext::mainWin=this;
    m_centerWdiget=new QWidget(this);
    m_vb=new QVBoxLayout();

    setCentralWidget(m_centerWdiget);

    m_centerWdiget->setLayout(m_vb);
    m_vb->setMargin(0);
    m_vb->setSpacing(0);
    this->setContentsMargins(0,0,0,0);
    m_vb->setContentsMargins(0,0,0,0);

    m_ld=new LoginDialog(this);
    m_ld->m_userinfo=&m_userinfo;
    connect(m_ld,SIGNAL(LoginWindowEvent()),this,SLOT(intoMeetingList()));
    m_tip=new YangDeskTip(this);
     m_tip->Init();

     m_tipThread=new YangTipThread();
     m_tipThread->m_tip=m_tip;

     m_tipThread->m_flashtime=3000;//3s


    m_meetinghandle=new YangMeetingInfoHandle();
    m_table=NULL;

    m_videos=new YangVideos();
    m_videos->m_win=this;
    m_videos->m_vb=m_vb;


    connect(m_videos,SIGNAL(QuitWindowEvent()),this,SLOT(on_exit()));
    QDesktopWidget *desktop = QApplication::desktop();
    m_ld->move((desktop->width() - m_ld->width())/ 2, (desktop->height() - m_ld->height()) /2);
    m_ld->show();

    m_videoThread=nullptr;
    m_sysMessage=nullptr;

    m_heartbeat=new QTimer(this);

    m_heartbeat->setInterval(30*1000);
    connect(m_heartbeat, SIGNAL(timeout()), this, SLOT(handleTimeout()));
    m_heartbeat->start();
}

MainWindow::~MainWindow()
{
    delete ui;
    destoryAll();
}
void MainWindow::initConf(){
    m_ini->audio.usingMono=1;
    m_ini->audio.sample=16000;
    m_ini->audio.channel=1;
    m_ini->audio.hasAec=1;
    m_ini->audio.audioCacheNum=10;
    m_ini->audio.audioCacheSize=10;
    m_ini->audio.audioPlayCacheNum=5;

    m_ini->video.videoCacheNum=10;
    m_ini->video.evideoCacheNum=10;
    m_ini->video.videoPlayCacheNum=10;

    /**
    m_ini->remoteAudio.encode=Yang_AED_OPUS;
    m_ini->remoteAudio.sample=16000;
    m_ini->remoteAudio.channel=1;
    m_ini->remoteAudio.clock=90000;

    m_ini->remoteVideo.encode=Yang_VED_264;
    m_ini->remoteVideo.clock=90000;
    **/
}
void MainWindow::initVideoThread(){

    YangVideoContext::yangThread=m_videoThread;
    m_ini->receive=this;
    m_ld->m_ini=m_ini;
    m_http=new YangHttpUtil(m_ini);
    m_ld->m_http=m_http;

    initPreview();
}
void MainWindow::closeEvent(QCloseEvent *event){
    destoryAll();
}

void MainWindow::destoryAll(){
    m_exitState=1;


    if(m_videoMeeting){
        m_videoMeeting->delMessage();
        m_tipThread->stopMessage();
        m_tipThread->m_tip=NULL;

        if(m_heartbeat) m_heartbeat->stop();
        yang_delete(m_heartbeat);
        m_videoMeeting->delMessage();
         m_videoThread->m_isLoop=0;
         //m_videoThread->m_tip=NULL;
         m_videoMeeting->stopAll();
         if(m_sysMessage) {
             qDebug()<<"m_sysMessage is stopped!..................";
             m_sysMessage->stop();
         }
        // yang_post_message(YangM_Sys_Shutdown,m_ini->user.userId,NULL);
         while (m_videoThread->m_isStart) {
             QThread::msleep(1);
         }

        m_videoThread->quit();
        m_videoThread=NULL;
        m_ini->receive=nullptr;
        YangVideoContext::mainWin=nullptr;
       // YangVideoContext::videoMeeting=nullptr;
        YangVideoContext::yangThread=nullptr;

         m_videoMeeting=NULL;
         yang_delete(m_tip);
        yang_delete(m_tipThread);
         yang_delete(m_vb);

         yang_delete(m_ld);
         yang_delete(m_http);

         yang_delete(m_tip);
         yang_delete(m_videos);

         if(m_sysMessage){
             while(m_sysMessage->m_isStart){
                 QThread::msleep(1);
             }
             m_sysMessage->deleteAll();
             delete m_sysMessage;
             m_sysMessage=nullptr;
         }

         yang_closeLogFile();
          yang_delete(m_ini);
    }
}
void MainWindow::handleTimeout(){
    if(m_videoMeeting) m_videoMeeting->sendHeartbeat();
}

void MainWindow::receiveMessage(char *pstr){

   // m_tip->showMessage(pstr);
}
void MainWindow::receiveError(int pet){

    qDebug()<<"...............System Error!............"<<pet;
    //QMessageBox::about(this, "网络错误", "网络错误，与服务器连接断开！");
    // m_tip->showMessage("网络错误，与服务器连接断开");
    // RDesktopTip::showTip( { "网络错误，与服务器连接断开！"} ,5); //只显示5s就消失
    //m_connHandle.start();
}

void MainWindow::intoMeeting(){
    yang_post_message(YangM_Sys_ConnectServer,m_ini->user.userId,NULL);
   // initIntoMeeting();
}
void MainWindow::handleConnectMeeting(int pret){
    QString estr;
    if(pret>0){
        //if(pret==Yang_SRTS_NONSRTSERVER)
         //   estr="连接SRT服务器连接失败！";
       // else
          //  estr="连接服务器连接失败！";

        estr="无法建立服务器,正在重新连接!";
        showMessage(estr);
        if(!m_exitState) QThread::sleep(3);
        if(!m_exitState) yang_post_message(YangM_Sys_ConnectServer,m_ini->user.userId,NULL);
        return;
    }
}
void MainWindow::initIntoMeeting(int pret){
 qDebug()<<"******initIntoMeeting********************************************"<<pret;


    m_videos->m_videos[0]->vwin1->vuid=m_ini->user.userId;
    if(m_ini->user.roomAdmin==1||m_ini->user.roomAdmin==3){
        yang_post_message(YangM_Sys_Pubaudio,m_ini->user.userId,NULL);
        yang_post_message(YangM_Sys_Pubvideo,m_ini->user.userId,NULL);
       // m_ini->user.micFlag=2;
       // m_ini->user.camFlag=2;
    }
    for(size_t i=0;i<m_videoMeeting->getUserList()->size();i++){
        if(m_videoMeeting->getUserList()->at(i)->userId!=m_ini->user.userId&&m_videoMeeting->getUserList()->at(i)->micFlag==2){
            yang_post_message(YangM_Sys_Playaudio,m_videoMeeting->getUserList()->at(i)->userId,NULL);
        }
    }
    QString errstr="用户"+QString(m_ini->user.userName)+"已进入教室:"+QString(m_ini->meeting.meetingName);
    // for(int i=0;i<4;i++){
    showMessage(errstr);  // RDesktopTip::showTip( { estr} ,1);
}

YangUser* MainWindow::getUser(int puid){
    for(size_t i=0;i<m_videoMeeting->getUserList()->size();i++){
        if(m_videoMeeting->getUserList()->at(i)->userId==puid) return m_videoMeeting->getUserList()->at(i);
    }
    return nullptr;
}

void MainWindow::handleConnectMedia(YangSysMessage* psm,int pret){
    QString estr;
      YangSysMessageType pmd=(YangSysMessageType)psm->messageId;

    if(pret){

        if(pmd==YangM_Sys_Pubaudio||pmd==YangM_Sys_Pubvideo||pmd==YangM_Sys_Playaudio||
                pmd==YangM_Sys_Playvideo||pmd==YangM_Sys_PushMediaServerConnect||pmd==YangM_Sys_PlayMediaServerConnect){
            estr="连接媒体服务器失败，重新连接!";
            qDebug()<<estr;
            showMessage(estr);
            if(!m_exitState) QThread::sleep(5);
             if(!m_exitState) yang_post_message(pmd,psm->uid,NULL);
        }

    }else{
        if(pmd==YangM_Sys_Login&&psm->uid!=m_ini->user.userId){
            YangUser *yu=getUser(psm->uid);
            if(yu){
             estr="用户"+QString(yu->userName)+"已进入教室:"+QString(m_ini->meeting.meetingName);
              qDebug()<<estr;
             showMessage(estr);
            }
        }
        if(pmd==YangM_Sys_Logout&&psm->uid!=m_ini->user.userId){
            qDebug()<<"...............handleConnectMedia..remove video..userid=="<<psm->uid;
            m_videos->unplayVideo(psm->uid);
            m_videoThread->delWd(psm->uid);
        }
    }
    if(pmd==YangM_Sys_PushMediaServerError){
        estr="连接媒体服务器失败，重新连接!";
        showMessage(estr);
        if(!m_exitState) QThread::sleep(5);
        if(!m_exitState) yang_post_message(YangM_Sys_PushMediaServerConnect,psm->uid,NULL);
    }
    if(pmd==YangM_Sys_PlayMediaServerError){
        estr="连接媒体服务器失败，重新连接!";
        showMessage(estr);
        if(!m_exitState) QThread::sleep(5);
        if(!m_exitState) yang_post_message(YangM_Sys_PlayMediaServerConnect,psm->uid,NULL);
    }
}
void MainWindow::intoMeetingList(){
    if(m_table==nullptr) {
        m_table=new MeetingTable(this);
        connect(m_table,SIGNAL(IntoMeetingEvent()),this,SLOT(intoMeeting()));
        m_table->m_ini=m_ini;
        m_table->m_http=m_http;
        m_table->m_userinfo=&m_userinfo;
        m_table->m_meeting=&m_ini->meeting;
        m_table->m_meetinghandle=m_meetinghandle;
        m_table->show();
        m_table->initTable();
    }

}
void MainWindow::initPreview(){
    if(m_isInit==0){

        m_videos->initVideos();
        int32_t showVideo=1;
        m_videos->m_videoType=showVideo;
        m_videos->initAll();
        m_videos->handleWin(1);
        QThread::usleep(500);
        m_videoThread->showWin(m_videos->m_videos[showVideo-1]->vwin1->getVWinId(),m_ini->user.userId);
        m_videos->cind=showVideo;
        m_isInit=1;
    }

}
void MainWindow::showMessage(QString str){
   if(1) return;
    m_tip->setMessage(str);
    if(m_tipThread&&!m_tipThread->getMessageState()) yang_post_message(YangM_Sys_ShowMessage,0,NULL);
}

void MainWindow::receiveSysMessage(YangSysMessage* psm,int pret){
    YangSysMessageType mt=(YangSysMessageType)psm->messageId;
    // printf("\n**receiveSysMessage*****pmessageId..type===%d,uid==%d",pmessageId,puid);
    //       printf("\n***receiveSysMessage**********pmessageId..type===%d,uid==%d",pmessageId,puid);

    qDebug()<<"receiveSysMessage=usesrId="<<psm->uid<<"=====type=="<<mt<<"...ret=="<<pret;
    switch (psm->messageId) {
    case YangM_Sys_Shutdown:break;
    case YangM_Sys_ConnectServer:handleConnectMeeting(pret);break;
    case YangM_Sys_DisconnectServer:break;
    case YangM_Sys_ConnectServerInterrupt:handleConnectMeeting(1);break;
    case YangM_Sys_PushMediaServerError:handleConnectMedia(psm,pret);break;
    case YangM_Sys_PlayMediaServerError:handleConnectMedia(psm,pret);break;
    case YangM_Sys_PushMediaServerConnect:handleConnectMedia(psm,pret);break;
    case YangM_Sys_PlayMediaServerConnect:handleConnectMedia(psm,pret);break;
    case YangM_Sys_Login:handleConnectMedia(psm,pret);break;
    case YangM_Sys_Logout:handleConnectMedia(psm,pret);break;
    case YangM_Sys_GetMeetinginfo:initIntoMeeting(pret);break;
    case YangM_Sys_Pubaudio:handleConnectMedia(psm,pret);break;
    case YangM_Sys_Pubvideo:handleConnectMedia(psm,pret);break;
    case YangM_Sys_UnPubaudio:break;
    case YangM_Sys_UnPubvideo:break;
    case YangM_Sys_Playaudio:handleConnectMedia(psm,pret);break;
    case YangM_Sys_Playvideo:handleConnectMedia(psm,pret);break;
    case YangM_Sys_UnPlayaudio:break;
    case YangM_Sys_UnPlayvideo:m_videoThread->delWd(psm->uid);break;
    case YangM_Sys_Setvr:break;
    case YangM_Sys_UnSetvr:break;
    }
   // emit YangSysMessageEvent();
}
void MainWindow::addMenu(int pind,QString str,QMenu *pMenu){
    QAction *pTask = new QAction(str, this);
    pTask->setData(pind);
    pMenu->addAction(pTask);
    connect(pTask, SIGNAL(triggered()), this, SLOT(onTaskBoxContextMenuHandle()));
}


void MainWindow::on_MainWindow_customContextMenuRequested(const QPoint &pos)
{
  //  qDebug()<<"--------------------------start context"<<m_videoMeeting->m_state;
    if(m_videoMeeting->m_state>0) return;
    QMenu *pMenu = new QMenu(this);
    int32_t strlen=m_ini->sys.isMultCamera?11:10;
    strlen+=m_ini->sys.isMultCamera?2:3;
    QString strs[]={"一分屏","二分屏","三分屏","四分屏","发言","禁言","发布视频","停止发布","退出教室","退出系统","切换视频","开始VR","结束VR"};
    for(int i=0;i<strlen;i++)    addMenu(i+1,strs[i],pMenu);
    // pMenu->exec(cursor().pos());    ;
    pMenu->exec(pos);
    QList<QAction*> list = pMenu->actions();
    foreach (QAction* pAction, list) delete pAction;
    delete pMenu;
}
void MainWindow::onTaskBoxContextMenuHandle(){
    QAction *pEven = qobject_cast<QAction *>(this->sender());
    // int32_t iType = pEven->data().toInt();
    m_videos->handleMenu(pEven->data().toInt());

}
void MainWindow::on_exit(){

   destoryAll();
    exit(0);

}
