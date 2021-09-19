#ifndef YANGSWIN_H
#define YANGSWIN_H

#include <QWidget>
//#include <QLabel>
//#include "yangselectuservideo.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/sys/YangSysMessageI.h"
class YangVideoShowWin : public QWidget
{
    Q_OBJECT
public:
    explicit YangVideoShowWin(QWidget *parent = nullptr);
     ~YangVideoShowWin();
public:
    //YangSelectUserVideo *m_selectUser;
   //  YangVideoTypeI *m_videoHandle;
   // int getVWinId();
   // int swId;
    //int vuid;
    //int uid
    QPaintEngine* paintEngine()const;
   // static int wids;


     //void success();
    // void failure(int errcode);


//void setWidgetBGColor();
protected:
//
     //   void resizeEvent(QResizeEvent *);
     //   void paintEvent(QPaintEvent *);
  // void mousePressEvent(QMouseEvent *);
private:
  //  YangPainter *m_ypainter;
   // QLabel *m_label;
private:
    int wid;
//private slots:
//void onVideoClick();
//signals:

};

#endif // YANGSWIN_H
