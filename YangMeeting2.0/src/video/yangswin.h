#ifndef YANGSWIN_H
#define YANGSWIN_H

#include <QWidget>
//#include <QLabel>
#include "yangselectuservideo.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/sys/YangSysMessageI.h"
#include "../yangplayer/YangPlayWidget.h"
class YangVideoShowWin : public YangPlayWidget,public YangSysMessageI
{
    Q_OBJECT
public:
    explicit YangVideoShowWin(QWidget *parent = nullptr);
     ~YangVideoShowWin();
public:
    YangSelectUserVideo *m_selectUser;
     YangVideoTypeI *m_videoHandle;
    int32_t getVWinId();
    int32_t swId;
    int32_t vuid;
    //int uid
    QPaintEngine* paintEngine()const;
   // static int32_t wids;


     void success();
     void failure(int errcode);


//void setWidgetBGColor();
protected:
//
     //   void resizeEvent(QResizeEvent *);
     //   void paintEvent(QPaintEvent *);
   void mousePressEvent(QMouseEvent *);
private:
  //  YangPainter *m_ypainter;
   // QLabel *m_label;
private:
    int32_t wid;
private slots:
void onVideoClick();
//signals:

};

#endif // YANGSWIN_H
