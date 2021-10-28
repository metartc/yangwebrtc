#ifndef YANGSWIN_H
#define YANGSWIN_H

#include <QWidget>
#include "yangselectuservideo.h"
#include "yangplayer/YangWinPlayFactroy.h"
#include "yangutil/sys/YangSysMessageI.h"
#include "../yangplayer/YangPlayWidget.h"

class YangVideoShowWin : public YangPlayWidget,public YangSysMessageI
{
    Q_OBJECT
public:
    explicit YangVideoShowWin(YangVideoContext* pcontext,QWidget *parent = nullptr);
     ~YangVideoShowWin();
public:
    YangSelectUserVideo *m_selectUser;
     YangVideoTypeI *m_videoHandle;
     YangVideoContext* m_context;
    int32_t getVWinId();
    int32_t swId;
    int32_t vuid;
    //int uid
    QPaintEngine* paintEngine()const;



     void success();
     void failure(int errcode);


protected:

   void mousePressEvent(QMouseEvent *);

private:
    int32_t wid;
private slots:
void onVideoClick();
//signals:

};

#endif // YANGSWIN_H
