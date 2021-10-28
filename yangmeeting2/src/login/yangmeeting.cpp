#include "yangmeeting.h"
#include <QtXml>
#include "yangmeeting/yangmeetingtype.h"
#include "../yangwinutil/yangvideocontext.h"
YangMeetingInfoHandle::YangMeetingInfoHandle(YangVideoContext* pcontext)
{

    m_users=nullptr;
   m_context=pcontext;
}

YangMeetingInfoHandle::~YangMeetingInfoHandle()
{
   // m_meeting=nullptr;
    if(m_users){
        for (int i=0;i< m_users->size();i++) {
            delete m_users->at(i);
        }
        m_users->clear();
    }
  m_context=nullptr;
}


void YangMeetingInfoHandle::success(QByteArray pa){
   //  qDebug()<<qPrintable(pa);
    QDomDocument doc;
    doc.setContent(pa);
    QDomElement root=doc.documentElement();
    QDomNode node1=root.firstChild();
    ret=1;
    //if(1) return;
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
    if(m_users==nullptr) m_users=new QList<YangUserInfo*>();
    // qDebug()<<"nodename=="<<root.nodeName()<<".."<<root.attribute("loginState");//.<<"=="<<root.toAttr().value();//["loginState"].value;
    while(!node1.isNull())  //如果节点不空
    {
        if(node1.isElement()) //如果节点是元素
        {
            QDomElement e1=node1.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多
           QDomNodeList cnodes= node1.childNodes();
          // meetings->push_back(new YangMeetings());
          // YangMeetings *mt=meetings->back();
           for (int x = 0; x < cnodes.count(); x++)
           {
               QDomElement e = cnodes.at(x).toElement();
                // qDebug()<<e.tagName()<<"==="<<e.text();
                 QString sr(e.tagName());

                  if(sr.compare("meetingId")==0){
                     m_meeting.meetingId=e.text();
                  }else if(sr.compare("meetingName")==0){
                      m_meeting.meetingName=e.text();
                  }else if(sr.compare("meetingType")==0){
                     m_meeting.meetingType=e.text().toInt();
                  }else if(sr.compare("meetingDesc")==0){
                      m_meeting.meetingDesc=e.text();
                  }else if(sr.compare("members")==0){
                    QDomNodeList cnodes1= e.childNodes();//node1.childNodes();
                    for (int x1 = 0; x1 < cnodes1.count(); x1++)
                    {
                        QDomElement e2 = cnodes1.at(x1).toElement();
                        m_users->push_back(new YangUserInfo());
                        YangUserInfo *yu=m_users->back();
                        yu->userId=e2.firstChildElement("memberId").text().toInt();
                        yu->userName=e2.firstChildElement("memberName").text();
                        yu->adminType=e2.firstChildElement("mtRole").text().toInt();
                       // yu->departId=
                      // qDebug()<<"*****userId="<<yu->userId<<"..name="<<yu->userName<<",adminType=="<<yu->adminType;
                        if(yu->userId==m_context->context->user.userId){

                            m_context->context->user.roomAdmin=yu->adminType;
                        }
                       // qDebug()<<"*****userId="<<yu->userId<<"..userName="<<yu->userName<<"..adminType="<<yu->adminType;//tagName()<<"==="<<e2.text();
                      //  QDomNodeList cnodes2=cnodes1.at(x1).childNodes();//e1.childNodes();
                       // for (int x2 = 0; x2 < cnodes2.count(); x1++)
                        //{
                        //    QDomElement e2 = cnodes2.at(x2).toElement();
                        //    qDebug()<<e2.tagName()<<"==="<<e2.text();
                       // }
                    }
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
void YangMeetingInfoHandle::failure(QString pa){
    ret=0;
    qDebug()<<qPrintable(pa);
}
