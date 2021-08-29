#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#ifdef _WIN32
#pragma execution_character_set("utf-8")
#endif
#include <QMainWindow>
#include <QVBoxLayout>
#include "src/video/yangvideothread.h"
#include "yangmeeting/yangmeetingtype.h"
#include "logindialog.h"
#include "meetingtable.h"
#include "src/login/yangUserInfo.h"
#include "src/login/yangmeetings.h"
#include "src/yangwinutil/yanghttputil.h"
#include "src/video/yangvideos.h"
#include "yangmeeting/YangMeetingHandle.h"
#include "src/yangwinutil/yangdesktip.h"
#include <QCloseEvent>
#include <QTimer>
#include "yangutil/sys/YangSysMessageHandle.h"
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

//struct YangSysMessageQt_type{
 //   int messageId;
 //   int uid;
  //  int ret;
//};

class MainWindow : public QMainWindow,public YangReceiveMessageI
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    YangSysMessageHandle *m_sysMessage;
    YangMeetingContext *m_ini;
    YangVideoThread *m_videoThread;
    YangUserInfo m_userinfo;
    YangHttpUtil *m_http;
    YangMeetingInfoHandle *m_meetinghandle;
    YangVideos *m_videos;
    YangMeetingHandle *m_videoMeeting;
    LoginDialog *m_ld;
    MeetingTable *m_table;
    YangDeskTip *m_tip;
    void initIni(YangMeetingContext *pini);
    void initIntoMeeting(int pret);
    void handleConnectMeeting(int pret);
    void handleConnectMedia(YangSysMessage* psm,int pret);
    void showMessage(QString str);
    void receiveError(int pet);
    void receiveMessage(char *pstr);
    void receiveSysMessage(YangSysMessage* psm,int pret);

    int m_exitState;
protected:
    void closeEvent(QCloseEvent *event);
private:
    Ui::MainWindow *ui;

    QTimer* m_heartbeat;
//signals:
 //   void YangShowMessage();
  // void YangSysMessageEvent();

protected slots:
    void on_MainWindow_customContextMenuRequested(const QPoint &pos);
    void onTaskBoxContextMenuHandle();

    void intoMeetingList();
    void intoMeeting();
    void on_exit();

    void handleTimeout();

    YangUser* getUser(int puid);
   // void onShowMessage();
   // void onSysmessage();
private:
    void initVideos();
    void initPreview();
    void initStyel();
    void destoryAll();
    void addMenu(int pind,QString str,QMenu *pMenu);
    QWidget *m_centerWdiget;
    QVBoxLayout *m_vb;
    // YangSWin *m_ys;

private:
   // YangSysMessageQt_type m_sysMessage;
    int m_isInit;
   // int m_isInitDecoder=0;


};
#endif // MAINWINDOW_H
