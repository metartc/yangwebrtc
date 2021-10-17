#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QHBoxLayout>
#include "yangpush/YangPushHandle.h"
#include "src/video/yangrecordthread.h"
//#include "src/video/yangrecordwin.h"
#include "src/video/yangrecordvideowin.h"
#include "src/video/YangPlayWidget.h"
#include "yangutil/yangavinfotype.h"
#include <yangutil/sys/YangSysMessageI.h>
QT_BEGIN_NAMESPACE
namespace Ui { class RecordMainWindow; }
QT_END_NAMESPACE

class RecordMainWindow : public QMainWindow,public YangSysMessageI
{
    Q_OBJECT

public:
    RecordMainWindow(QWidget *parent = nullptr);
    ~RecordMainWindow();

    YangPushHandle *m_rec;
    YangContext *m_ini;
    YangRecordThread* m_rt;


    YangPlayWidget *m_win0;
    QHBoxLayout *m_hb0;
public:

    void success();
    void failure(int32_t errcode);
     void initVideoThread(YangRecordThread *prt);
    void closeEvent( QCloseEvent * event );
private slots:
    void on_m_b_rec_clicked();
    void on_m_c_vr_clicked();
    void on_m_c_screen_clicked();

private:
    void init(YangContext *para);
    Ui::RecordMainWindow *ui;
    void initPreview();
    void closeAll();


private:
    int32_t m_isStartpush;
    int32_t m_isVr;




};
#endif // MAINWINDOW_H
