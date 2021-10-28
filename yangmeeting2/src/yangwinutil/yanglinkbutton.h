#ifndef YANGLINKBUTTON_H
#define YANGLINKBUTTON_H

#include<QWidget>
#include<QCommandLinkButton>
#include "yangqInterfaces.h"
class YangLinkButton:public QCommandLinkButton
{
    Q_OBJECT
public:
    YangLinkButton(QWidget *parent=nullptr);
    ~YangLinkButton();
    int32_t m_index=0;
    YangLinkButtonI *m_lbb;
private slots:
  void  clickbutton();

};

#endif // YANGLINKBUTTON_H
