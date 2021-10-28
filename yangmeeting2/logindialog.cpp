#include "logindialog.h"
#include "src/yangwinutil/yangvideocontext.h"
#include "ui_logindialog.h"
#include <QMessageBox>
//#include "src/popup/RMessageDialog.h"
LoginDialog::LoginDialog(YangVideoContext* pcontext,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LoginDialog)
{
    ui->setupUi(this);
    //  setWindowFlags(Qt::CustomizeWindowHint|Qt::FramelessWindowHint);
    setWindowFlags(Qt::FramelessWindowHint);
    // setAttribute(Qt::WA_StyledBackground);
   //  setWindowFlags(Qt::WindowStaysOnTopHint);
    // this->repaint();
    m_http=nullptr;

    m_context=pcontext;
    this->setStyleSheet(R"(
                        .LoginDialog QLabel{
                        color:white;
                        border: 1px solid white;
                        padding:0;
                        border-radius:5px;
                        background-color:rgba(250,170,0,150);
                        }
                        )");
}

LoginDialog::~LoginDialog()
{
    delete ui;
    m_http=nullptr;


}

void LoginDialog::on_pushButton_2_clicked()
{
    QString userName=ui->userName->text().trimmed();
    QString passwd2=ui->password->text().trimmed();
    QByteArray pass1=passwd2.toUtf8();
    QString passwd;
    passwd.prepend(pass1.toBase64());
    if(userName.length()==0){
        QMessageBox::about(this,"请输入用户名","请输入用户名！");
        //RMessageDialog::showMessage("请输入用户名","请输入用户名!");
        return;
    }
    if(passwd.length()==0){
        QMessageBox::about(this,"请输入密码","请输入密码!");
       // RMessageDialog::showMessage("请输入密码","请输入密码!");
        return;
    }
    YangUserHandle yuh;
    yuh.m_userinfo=m_context->userinfo;

    yuh.loginState=0;
    QString param="loginName="+userName+"&"+"pwd="+passwd+"&opt=loginverify&moduleId=user";
    qDebug()<<"..param==="<<param;
    m_http->getPost("mthandle",param,&yuh);

    if(yuh.loginState==2){
        QMessageBox::about(this,"密码错误","用户名或密码不正确！");
    }else if(yuh.loginState==Yang_Ok){
        // qDebug()<<"loginStart===="<<m_user->loginState<<"...userId=="<<m_user->userId<<"..adminId=="<<m_user->adminId;
        YangUser *user=&m_context->videoMeeting->m_context->user;
        user->userId=yuh.m_userinfo->userId;
        memset(user->userName,0,sizeof(user->userName));
        memset(user->loginName,0,sizeof(user->loginName));
        sprintf(user->userName,"%s",yuh.m_userinfo->userName.toUtf8().data());
        sprintf(user->loginName,"%s",yuh.m_userinfo->loginName.toUtf8().data());
        m_context->context->user.camFlag=1;
        m_context->context->user.micFlag=1;
        m_context->context->user.roomAdmin=0;
      //   qDebug()<<".>>>>>>>>>>>>>>>>..userId=="<<user->userId;

        user=NULL;
        emit LoginWindowEvent();
        this->hide();
        return;
    }else if(yuh.loginState==1){
        QMessageBox::about(this,"网络错误","网络错误！");
    }
    // qDebug()<<"loginStart===="<<m_user->loginState<<"...userId=="<<m_user->userId<<"..adminId=="<<m_user->adminId;
    //m_http->getHttp();
}

void LoginDialog::on_pushButton_clicked()
{
    hide();
}




void LoginDialog::on_password_returnPressed()
{
    on_pushButton_2_clicked();
}
