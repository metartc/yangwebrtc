#include "yangmeetings.h"
#include <QtXml>

YangMeetingsHandle::YangMeetingsHandle()
{
    meetings=NULL;
}

YangMeetingsHandle::~YangMeetingsHandle()
{
    meetings=NULL;
}


void YangMeetingsHandle::success(QByteArray pa){
    //qDebug()<<qPrintable(pa);
    //printf("%s",pa.toStdString().c_str());
    QDomDocument doc;
    doc.setContent(pa);
    QDomElement root=doc.documentElement();
    QDomNode node1=root.firstChild();
    ret=1;
   /** if(root.hasAttribute("loginState")){
        QString qs(root.attribute("loginState"));
        qDebug()<<"nodename=="<<root.nodeName()<<".."<<root.attribute("loginState");
        if(qs.trimmed().compare("success")==0){
            ret=2;
        }else{
            ret=1;
            return;
        }

    }else{
        return;
    }**/

    // qDebug()<<"nodename=="<<root.nodeName()<<".."<<root.attribute("loginState");//.<<"=="<<root.toAttr().value();//["loginState"].value;
    while(!node1.isNull())  //如果节点不空
    {
        if(node1.isElement()) //如果节点是元素
        {
            QDomElement e1=node1.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
           QDomNodeList cnodes= node1.childNodes();
           meetings->push_back(new YangMeetingInfo());
           YangMeetingInfo *mt=meetings->back();
           for (int x = 0; x < cnodes.count(); x++)
           {
               QDomElement e = cnodes.at(x).toElement();
               //  qDebug()<<e.tagName()<<"==="<<e.text();
                 QString sr(e.tagName());

                  if(sr.compare("meetingId")==0){
                      mt->meetingId=e.text();
                  }else if(sr.compare("meetingName")==0){
                      mt->meetingName=e.text();
                  }else if(sr.compare("meetingType")==0){
                      mt->meetingType=e.text().toInt();
                  }else if(sr.compare("meetingDesc")==0){
                      mt->meetingDesc=e.text();
                  }else if(sr.compare("adminName")==0){
                      mt->adminName=e.text();
                  }
               // Do something with node
           }
           //  qDebug()<<e.tagName()<<"==="<<e.text(); //打印键值对，tagName和nodeName是一个东西
           // QString sr(e.tagName());


            //QDomNodeList list=e.childNodes();
            /** for(int i=0;i<list.count();i++) //遍历子元素，count和size都可以用,可用于标签数计数
               {
                   QDomNode n=list.at(i);
                   if(node.isElement())
                       qDebug()<<n.nodeName()<<":"<<n.toElement().text();
               }**/
        }
        node1=node1.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
    }

}
void YangMeetingsHandle::failure(QString pa){
    ret=0;
    qDebug()<<qPrintable(pa);
}
