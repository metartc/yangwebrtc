#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QVBoxLayout>
#include <QMainWindow>
#include <string>
//#include "src/video/yangswin.h"
#include "src/video/yangrecordthread.h"
#include "yangplayer/YangPlayerHandle.h"
#include "yangstream/YangStreamType.h"
#include "src/yangplayer/YangPlayWidget.h"
#include <yangutil/yangavinfotype.h>
QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    YangRecordThread *m_videoThread;
    YangPlayerHandle *m_player;
    YangPlayWidget *m_videoWin;
    YangStreamConfig m_conf;
    void initVideoThread(YangRecordThread *prt);
protected:
    YangContext* m_ini;
private slots:
void on_m_b_play_clicked();

private:
    Ui::MainWindow *ui;

    //std::string serverIp,app,stream;
   // std::string localServer;
    int localPort;
    QWidget *m_centerWdiget;
    QVBoxLayout *m_vb;
    QHBoxLayout *m_hb1;
    QHBoxLayout *m_hb2;
private:
    bool m_isStartplay;

};
#endif // MAINWINDOW_H
