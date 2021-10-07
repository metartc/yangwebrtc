#ifndef YANGTHREAD1_H
#define YANGTHREAD1_H
#include <QThread>
#include <QVector>

#include <yangutil/buffer/YangVideoBuffer.h>
#include <src/yangplayer/YangPlayWidget.h>
#include <yangstream/YangSynBuffer.h>
class YangRecordThread : public QThread
{
public:
    YangRecordThread();
    virtual ~YangRecordThread();

    int m_isLoop;
    void initPara();
    YangPlayWidget *m_video;
    YangSynBuffer* m_syn;

    int m_sid;
    void stopAll();
    int showType;

private:
    void render();
    void closeAll();

    YangFrame m_frame;
    int m_isStart;





private:
    virtual void run();
};

#endif // YANGTHREAD_H
