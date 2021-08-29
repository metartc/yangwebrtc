#ifndef LOGINDIALOG_H
#define LOGINDIALOG_H

#include <QDialog>
#include "src/yangwinutil/yanghttputil.h"
#include "src/login/yangUserInfo.h"
namespace Ui {
class LoginDialog;
}

class LoginDialog : public QDialog
{
    Q_OBJECT

public:
    explicit LoginDialog(QWidget *parent = nullptr);
    ~LoginDialog();
    YangHttpUtil *m_http;
    YangMeetingContext *m_ini;
    YangUserInfo *m_userinfo;
    //YangUser* m_user;
 signals:
    void LoginWindowEvent();

private slots:
    void on_pushButton_2_clicked();
    void on_pushButton_clicked();

    void on_password_returnPressed();

private:
    Ui::LoginDialog *ui;



};

#endif // LOGINDIALOG_H
