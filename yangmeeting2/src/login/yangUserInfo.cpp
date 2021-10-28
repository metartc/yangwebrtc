#include "yangUserInfo.h"
#include <QtXml>
YangUserHandle::YangUserHandle()
{
    m_userinfo=nullptr;
}
YangUserHandle::~YangUserHandle(){
    m_userinfo=nullptr;
}


void YangUserHandle::success(QByteArray pa){
    // qDebug()<<qPrintable(pa);
    QDomDocument doc;
    doc.setContent(pa);
    QDomElement root=doc.documentElement();
    QDomNode node=root.firstChild();
    if(root.hasAttribute("loginState")){
        QString qs(root.attribute("loginState"));
      //  qDebug()<<"nodename=="<<root.nodeName()<<".."<<root.attribute("loginState");
        if(qs.trimmed().compare("success")==0){
            loginState=0;
        }else{
            loginState=2;
            return;
        }

    }else{
        return;
    }
    // qDebug()<<"nodename=="<<root.nodeName()<<".."<<root.attribute("loginState");//.<<"=="<<root.toAttr().value();//["loginState"].value;
    while(!node.isNull())  //如果节点不空
    {
        if(node.isElement()) //如果节点是元素
        {
            QDomElement e=node.toElement(); //转换为元素，注意元素和节点是两个数据结构，其实差不多

            // qDebug()<<e.tagName()<<"==="<<e.text(); //打印键值对，tagName和nodeName是一个东西
            QString sr(e.tagName());
            if(sr.compare("userId")==0){

                m_userinfo->userId=e.text().toInt();
                // qDebug()<<e.tagName()<<"....text=="<<e.text()<<"...userId==="<<userId;
            }else if(sr.compare("loginName")==0){
                m_userinfo->loginName=e.text();
            }else if(sr.compare("adminId")==0){
                m_userinfo->adminId=e.text().toInt();
            }else if(sr.compare("userName")==0){
                m_userinfo->userName=e.text();
            }//else if(sr.compare("departId")==0){
               // m_userinfo->departId=e.text().toInt();
            //}

            //QDomNodeList list=e.childNodes();
            /** for(int i=0;i<list.count();i++) //遍历子元素，count和size都可以用,可用于标签数计数
               {
                   QDomNode n=list.at(i);
                   if(node.isElement())
                       qDebug()<<n.nodeName()<<":"<<n.toElement().text();
               }**/
        }
        node=node.nextSibling(); //下一个兄弟节点,nextSiblingElement()是下一个兄弟元素，都差不多
    }

}
void YangUserHandle::failure(QString pa){
    loginState=1;
    qDebug()<<qPrintable(pa);
}
