#ifndef YANGHTTPUTIL_H
#define YANGHTTPUTIL_H
#include <QNetworkAccessManager>
#include <QWidget>
#include "yangmeeting/yangmeetingtype.h"
#include "yangqInterfaces.h"
class YangHttpUtil:public QWidget
{
public:
    YangHttpUtil(YangMeetingContext *pini);
     ~YangHttpUtil();
    QString m_url,m_ip;
    int32_t m_port;
    void init(YangMeetingContext *pini);
   // void setBase(YangHttpBase *phb);
public:
   // int32_t getHttp(QString qs);
    int32_t getPost(QString module,QString param,YangHttpI *phb);
public slots:
void finished(QNetworkReply *reply);
private:
   // QNetworkAccessManager* m_net;
   // YangHttpBase *m_hb;
};

#endif // YANGHTTPUTIL_H
