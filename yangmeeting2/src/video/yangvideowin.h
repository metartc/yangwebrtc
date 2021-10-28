#ifndef YANGVIDEOWIN_H
#define YANGVIDEOWIN_H
#include <QDialog>
#include <QHBoxLayout>
#include "yangswin.h"
#include "../yangwinutil/yangqInterfaces.h"
#include "yangselectuservideo.h"
#include "yangvideotype.h"
#include "../yangplayer/YangPlayWidget.h"
class YangVideoWin: public QWidget,public YangVideoTypeI
{
Q_OBJECT
public:
    YangVideoWin(YangVideoContext* pcontext,QWidget *parent = nullptr);
    virtual ~YangVideoWin();
    YangSelectUserVideo *m_selectUser;
    QHBoxLayout *m_hb;
    YangVideoShowWin *vwin1,*vwin2,*vwin3,*vwin4;
    QVBoxLayout *m_vb1;
    QVBoxLayout *m_vb2;
    int32_t sid;
    void init();


    void clearVideo();
    void removeVideo(int puid);

    void addShowVideo();


protected:
    YangVideoContext* m_context;
   // void showEvent(QShowEvent *event);
private slots:
    //void onTaskBoxContextMenuHandle();
   // void initContextMenu(const QPoint &pos);
   // void onVideoClick();
private:
    void init(int pind);
    void init1();
    void init2();
    void init3();
    void init4();
    void initSelect();
    QWidget *m_widget1,*m_widget2;

};

#endif // YANGVIDEOWIN_H
