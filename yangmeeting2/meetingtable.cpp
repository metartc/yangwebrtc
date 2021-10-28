#include "meetingtable.h"
#include "ui_meetingtable.h"
#include <QStandardItemModel>
#include <QMessageBox>
#include <QCommandLinkButton>
#include <QHeaderView>
#include "src/yangwinutil/yanglinkbutton.h"
#include "src/yangwinutil/yangvideocontext.h"
MeetingTable::MeetingTable(YangVideoContext* pcontext,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MeetingTable)
{
    ui->setupUi(this);
    m_http=nullptr;
     m_context=pcontext;
   // m_meeting=nullptr;

    m_meetinghandle=NULL;

    m_vb=new QVBoxLayout(this);
    m_table=new QTableWidget(this);
    setLayout(m_vb);
    m_vb->addWidget(m_table);
    m_vb->setAlignment(Qt::AlignCenter);
    m_table->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

MeetingTable::~MeetingTable()
{
    delete ui;
    m_http=nullptr;
    m_context=NULL;

   // m_meeting=nullptr;
    for(int i=0;i<meetings.size();i++){
        delete meetings.at(i);
    }
    meetings.clear();

}

void MeetingTable::setIndex(int pindex){
    m_selIndex=pindex;

    QString param="meetingId="+meetings[m_selIndex]->meetingId+"&opt=select&moduleId=meeting";
    m_http->getPost("mthandle",param,m_meetinghandle);
    if(m_meetinghandle->ret==0){
        QMessageBox::about(this,"网络错误","网络错误！");
    }else{
         this->hide();
        if(m_context->meeting){
            m_context->meeting->meetingId=m_meetinghandle->m_meeting.meetingId.toInt();
            memset(m_context->meeting->meetingName,0,sizeof(m_context->meeting->meetingName));
            sprintf(m_context->meeting->meetingName,"%s",m_meetinghandle->m_meeting.meetingName.toUtf8().data());

        }
        emit IntoMeetingEvent();
        return;
    }
}
void MeetingTable::initTable(){
    YangMeetingsHandle ymh;
    ymh.meetings=&meetings;
    QString param="userId="+QString::number(m_context->userinfo->userId)+"&opt=selectsdoor&moduleId=meeting";
    m_http->getPost("mthandle",param,&ymh);
    int32_t cou=meetings.size();
    if(ymh.ret==1){
        //QMessageBox::about(this,"密码错误","用户名或密码不正确！");
    }else{
        QMessageBox::about(this,"网络错误","网络错误！");
    }
    // m_table->clear();
    m_table->setShowGrid(true);
    m_table->setGridStyle(Qt::DotLine);
    m_table->setColumnCount(4);
    m_table->setRowCount(cou);

    QStringList header;
    header<<"教学编号"<<"教学主题"<<"发起人"<<" ";
    m_table->setHorizontalHeaderLabels(header);

    for(int i=0;i<cou;i++){
        YangMeetingInfo *ym=meetings[i];
        m_table->setItem(i, 0, new QTableWidgetItem(ym->meetingId));
        m_table->item(i,0)->setTextAlignment(Qt::AlignHCenter);
        m_table->item(i,0)->setTextAlignment(Qt::AlignHCenter);
        m_table->setItem(i, 1, new QTableWidgetItem(ym->meetingName));
        m_table->item(i,1)->setTextAlignment(Qt::AlignHCenter);
        m_table->setItem(i, 2, new QTableWidgetItem(ym->adminName));
        m_table->item(i,2)->setTextAlignment(Qt::AlignHCenter);
        YangLinkButton *qlb=new YangLinkButton(this);
        qlb->m_index=i;
        qlb->m_lbb=this;
        qlb->setText("进入");
        m_table->setCellWidget(i,3,qlb);
        //  m_table->item(i,3)->setTextAlignment(Qt::AlignHCenter);


    }

    m_table->setSelectionBehavior(QAbstractItemView::SelectRows);
    m_table->show();
}

