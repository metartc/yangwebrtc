#ifndef YANGDESKTIP_H
#define YANGDESKTIP_H

#include <QWidget>
//#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QThread>
#include <QVector>
class YangDeskTip : public QWidget
{   
public:
    explicit YangDeskTip(QWidget *parent = nullptr);
    ~YangDeskTip();
    void showMessage();
    void Init();
    void initShow();
    void setMessage(QString message);
    int32_t flashMove();
    void setFlashTime(int flashtime);//milli second
    int32_t getWaitTime();//milli second


private:
    int32_t m_flashtime;
     void clearText();
private:
    QString m_text;
    QVector<QString> *m_stringList;
    int32_t m_Width,m_Height;
    int32_t m_nDesktopHeight,m_nWindowHeight;
    QPoint m_point;
    QLabel *m_message,*m_label;
    QWidget *m_widget,*m_widget_1,*m_widget_2;
    int32_t flashWight;
    int32_t fixHeight;
    int32_t ret;
};

#endif // YANGDESKTIP_H
