#ifndef YANGHTTPBASE_H
#define YANGHTTPBASE_H
#include <QByteArray>
#include <QNetworkAccessManager>
struct YangHttpI{
    virtual void success(QByteArray pa)=0;
    virtual void failure(QString pa)=0;
};
struct YangLinkButtonI{
     virtual void setIndex(int pindex)=0;
};
struct YangMenuHandleI{
    virtual void handleMenu(int ptype)=0;
};
#endif // YANGHTTPBASE_H
