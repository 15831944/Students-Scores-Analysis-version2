#include "managewidget.h"
#include "ui_login.h"

ManageWidget::ManageWidget(QWidget *parent):QWidget(parent)
{
    lg = new Login;
    w = new MainWindow;
    about = new About;

    //�����ޱ߿��¼����
    lg->setWindowTitle(tr("������ҵ��ѧ�ۺϲ���ϵͳ"));
    lg->setWindowOpacity(1);
    lg->setWindowFlags(Qt::FramelessWindowHint);
    lg->setAttribute(Qt::WA_TranslucentBackground);

    lg->show();

    QObject::connect(lg,SIGNAL(enter()),this,SLOT(showWindow()));
    //QObject::connect(w,SIGNAL(logout()),this,SLOT(showLogin()));
    QObject::connect(lg,SIGNAL(help()),this,SLOT(showAbout()));
}

void ManageWidget::showWindow()
{
    lg->hide();
    w->show();
}
void ManageWidget::showLogin()
{
    w->close();
    delete lg;
    delete w;
    delete about;
    lg = new Login;
    w = new MainWindow;
    about = new About;

    //�����ޱ߿��¼����
    lg->setWindowTitle(tr("������ҵ��ѧ�ۺϲ���ϵͳ"));
    lg->setWindowOpacity(1);
    lg->setWindowFlags(Qt::FramelessWindowHint);
    lg->setAttribute(Qt::WA_TranslucentBackground);

    lg->show();
    QObject::connect(lg,SIGNAL(enter()),this,SLOT(showWindow()));
    QObject::connect(w,SIGNAL(logout()),this,SLOT(showLogin()));
    QObject::connect(lg,SIGNAL(help()),this,SLOT(showAbout()));
}
ManageWidget::~ManageWidget()
{
    delete lg;
    delete w;
    delete about;
}

void ManageWidget::showAbout()
{
    //�����ޱ߿��¼����
    about->setWindowTitle(tr("������ҵ��ѧ�ۺϲ���ϵͳ"));
    about->setWindowOpacity(1);
    about->setWindowFlags(Qt::FramelessWindowHint);
    about->setAttribute(Qt::WA_TranslucentBackground);

    about->show();
}


