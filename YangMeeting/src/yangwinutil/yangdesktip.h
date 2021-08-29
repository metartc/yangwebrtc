#ifndef YANGDESKTIP_H
#define YANGDESKTIP_H

#include <QWidget>
//#include <QTimer>
#include <QPushButton>
#include <QLabel>
#include <QThread>
class YangDeskTip : public QWidget
{   
    // Q_OBJECT
public:
    explicit YangDeskTip(QWidget *parent = nullptr);
    ~YangDeskTip();
    void showMessage();
    void showMessage(QString message);
    void showMessageStay(QString message);
    void Init();
    void setTitle(QString title, QIcon icon);
    void setMessage(QString message);
    void initlable( QLabel* plabel);
    QString m_text;

//public slots:
   // void onShowMessage();
   // void onClickClose();
    //void onMove();
  //  void onStay();
    //void onClose();


//signals:
    //void MaskWidgetHide();
   // void YangShowMessage();
public:
    int m_Width,m_Height;

     QPushButton *m_close;
    QWidget *m_widget,*m_widget_1,*m_widget_2;
  //  QPushButton *m_title,*m_close;
    QLabel *m_message,*m_label;
    int m_nDesktopHeight,m_nWindowHeight;
    QPoint m_point;

   // QStringList *m_stringList;


   // QTimer *m_pShowTimer,*m_pStayTimer,*m_pCloseTimer;

};

#endif // YANGDESKTIP_H
