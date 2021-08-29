#include "yangselectuservideo.h"
#include "yangutil/yangtype.h"
#include "../yangwinutil/yanglinkbutton.h"
#include <QStandardItem>
#include <QDebug>
#include <QHeaderView>
#include <vector>
#include "../yangwinutil/yangcontext.h"
//#include "../yangutil/yangmeetingtype.h"
#include "YangVideoWinHandleH.h"
using namespace std;
YangSelectUserVideo::YangSelectUserVideo(QWidget *parent):QDialog(parent)
{
    m_table=new QTableWidget(this);
    m_hb=new QHBoxLayout(this);
    this->setLayout(m_hb);
    m_hb->addWidget(m_table);
    m_widget=nullptr;
    m_videoHandle=nullptr;
    m_uid=nullptr;
    wid=0;
    m_message=nullptr;
    //this->setLayout(m_view);

}


YangSelectUserVideo::~YangSelectUserVideo(){
    m_videoHandle=nullptr;
    m_uid=nullptr;
     m_widget=nullptr;
     m_message=nullptr;
    yang_delete(m_hb);
    yang_delete(m_table);
}

void YangSelectUserVideo::setIndex(int pindex){

    vector<YangUser*> *yus=YangVideoContext::mainWin->m_videoMeeting->getUserList();
    int uid=yus->at(pindex)->userId;

    if(uid==*m_uid) {yus=NULL;this->hide();return;}
    if(m_videoHandle) m_videoHandle->removeVideo(uid);
    if(*m_uid>-1)         YangVideoContext::yangThread->delWd(*m_uid);

    *m_uid=uid;
    if(uid==YangVideoContext::mainWin->m_ini->user.userId){
        yang_add_winshowvideo(wid,uid);

    }else{
        yang_post_message(YangM_Sys_Playvideo,uid,m_message);
    }
    yus=NULL;
    this->hide();

}

void YangSelectUserVideo::init(){

    vector<YangUser*> *yus=YangVideoContext::mainWin->m_videoMeeting->getUserList();
    if(yus==NULL) return;
        if(m_table) m_table->clear();
        m_list.clear();
        m_table->setShowGrid(true);
        m_table->setGridStyle(Qt::DotLine);

        for(int j=0;j<(int)yus->size();j++){
            YangUser *yu=yus->at(j);
            qDebug()<<"name=="<<yus->at(j)->userName<<"..camFlag=="<<yus->at(j)->camFlag<<"..micFlag=="<<yus->at(j)->micFlag;
           // printf("name==%s,camFlag==%d",yus->at(j)->userName,yus->at(j)->camFlag);
           if(yu->userId==YangVideoContext::mainWin->m_ini->user.userId||yu->camFlag==2) m_list.push_back(QString(yu->userName));
           yu=NULL;
        }

        int cou=m_list.size();
        m_table->setRowCount(cou);
        m_table->setColumnCount(2);
        for(int i=0;i<cou;i++){
            QString ym=m_list[i];
            m_table->setItem(i, 0, new QTableWidgetItem(ym));

            YangLinkButton *qlb=new YangLinkButton(this);
            qlb->m_index=i;
            qlb->m_lbb=this;
            qlb->setText("查看");
            m_table->setCellWidget(i,1,qlb);

        }
        //m_table->setModel(model);
        //  m_table->setSelectionModel(selectModel);
        m_table->horizontalHeader()->setVisible(false);
        m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
        m_table->show();
       // m_init=1;
   // }

}
