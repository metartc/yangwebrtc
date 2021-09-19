#include "recordmainwindow.h"
#include "ui_recordmainwindow.h"

#include "yangrecliving/YangLivingType.h"
#include <QDebug>
#include "yangutil/sys/YangLog.h"
#include "yangutil/sys/YangFile.h"
#include <yangutil/sys/YangSocket.h>
#include <yangutil/sys/YangIni.h>
#include <QMessageBox>
//#include "yangrecliving/YangLivingMessageHandle.h"
RecordMainWindow::RecordMainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::RecordMainWindow)
{

    ui->setupUi(this);

    m_ini=new YangContext();
    memset(m_ini, 0, sizeof(m_ini));
    init_context(m_ini,"yang_config.ini");

    yang_setLogLevle(m_ini->sys.logLevel);
    yang_setLogFile(m_ini->sys.hasLogFile);
    init(m_ini);

    //m_showType=1;

    m_isStartpush=0;

    m_win0=NULL;
     m_isVr=0;
    m_rec=YangPushHandle::createPushHandle(m_ini);
    m_rec->init();

    m_hb0=new QHBoxLayout();
    ui->vdMain->setLayout(m_hb0);
    m_win0=new YangPlayWidget(this);
    m_hb0->addWidget(m_win0);
    m_hb0->setMargin(0);
    m_hb0->setSpacing(0);
    YangSocket su;
    char s[128]={0};
    sprintf(s,"webrtc://%s:1985/live/livestream",su.getLocalInfo().c_str());
    ui->m_url->setText(s);

}

RecordMainWindow::~RecordMainWindow()
{
    closeAll();

}
void RecordMainWindow::init_context(YangContext* context,char* filename){
    YangIni ini;
    ini.init(filename);
    ini.initAudio(&context->audio);
    ini.initVideo(&context->video);
    ini.initSys(&context->sys);
    ini.initEnc(&context->enc);
    ini.initRtc(&context->rtc);
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
        //  m_timer->stop();
        m_isStartpush=!m_isStartpush;
    }
    if(m_rec){
        if(m_isStartpush){
            m_rec->disconnect();
            //  m_timer->stop();
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


void RecordMainWindow::init(YangContext *para) {
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


}



void RecordMainWindow::on_m_b_rec_clicked()
{
    if(!m_isStartpush){

        ui->m_b_rec->setText("停止");
        // m_rec->recordFile();
        //yang_post_message(YangM_Rec_Start,0,NULL);
        // m_timer->start(1000);
        m_isStartpush=!m_isStartpush;
        if(m_rec) {
            YangSocket su;
               char s[128]={0};
               sprintf(s,"webrtc://%s:1985/live/livestream",su.getLocalInfo().c_str());
            m_rec->publish(ui->m_url->text().toStdString(),s,m_ini->sys.rtcLocalPort);
        }
        // m_recTimeLen=0;
    }else{
        ui->m_b_rec->setText("开始");
        if(m_rec) m_rec->disconnect();
        // m_rec->stopRecord();
        // yang_post_message(YangM_Rec_Stop,0,NULL);
        // m_timer->stop();
        m_isStartpush=!m_isStartpush;
    }

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

}
