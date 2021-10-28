#include "yanghttputil.h"
#include <QNetworkReply>
#include <QEventLoop>
YangHttpUtil::YangHttpUtil(YangMeetingContext *pini)
{
    //m_net=new QNetworkAccessManager();
     //connect(m_net, SIGNAL(finished(QNetworkReply*)), this, SLOT(finished(QNetworkReply*)));
    // m_hb=NULL;
    m_ip=QString(pini->sys.httpServerIP);
    m_port=pini->sys.httpPort;
   // qDebug()<<"app====="<<pini->hd.app;
    m_url=QString::asprintf("http://%s:%d/%s",pini->sys.httpServerIP,m_port,pini->hd.app);


}

YangHttpUtil::~YangHttpUtil(){
   // if(m_net) delete m_net;
   // m_net=NULL;
   // if(m_hb!=NULL) delete m_hb;
   // m_hb=NULL;
}
//void YangHttpUtil::setBase(YangHttpBase *phb){
  //  m_hb=phb;
//}
//int YangHttpUtil::getHttp(QString qs){
  /**  QNetworkRequest request;
    request.setUrl(QUrl(qs));
    m_net->get(request);
    QByteArray postData;
    postData.append("username=admin&password=123456");
    QEventLoop eventLoop;
    m_net->post(request,postData);
    eventLoop.exec();**/
    //post
   // QNetworkReply* reply = m_net->post(request, postData);
//return 1;

//}
int YangHttpUtil::getPost(QString module,QString param,YangHttpI *phb){
    int32_t ret=0;
    QNetworkAccessManager qam;
    QNetworkRequest request;
    QUrl url(m_url+"/"+module);
    request.setUrl(url);
   // request.setRawHeader("Content-Type","application/json");application/x-www-form-urlencoded
    request.setRawHeader("Content-Type","application/x-www-form-urlencoded");
    QByteArray postData;
    postData.append(param);
   // qDebug()<<"url===="<<url.url()<<"...."<<m_url+"/"+module;
    QEventLoop eventLoop;
   QNetworkReply *reply= qam.post(request,postData);
    connect(reply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
 //qDebug()<<".............start http1";
    eventLoop.exec();
    //qDebug()<<".............start http2";
    if (reply->error() == QNetworkReply::NoError)
    {
       // QByteArray bytes = reply->readAll();
       // if(phb) phb->success(reply->readAll().replace("\"",""));
         if(phb) phb->success(reply->readAll());
       // qDebug() << bytes;
        ret=1;
    }
    else
    {
        if(phb) phb->failure(reply->errorString());
        ret=2;
       // qDebug() << "finishedSlot errors here";
       // qDebug( "found error .... code: %d\n", (int)reply->error());
      //  qDebug(qPrintable(reply->errorString()));
    }
    reply->deleteLater();
    reply=NULL;
    return ret;

}

void YangHttpUtil::finished(QNetworkReply *reply)
{
     if (reply->error() == QNetworkReply::NoError)
     {
        // QByteArray bytes = reply->readAll();
         //if(m_hb) m_hb->success(reply->readAll());
        // qDebug() << reply->readAll();
     }
     else
     {
        // if(m_hb) m_hb->failure(reply->errorString());
        // qDebug() << "finishedSlot errors here";
        // qDebug( "found error .... code: %d\n", (int)reply->error());
        // qDebug(reply->errorString());
     }
     reply->deleteLater();
}
