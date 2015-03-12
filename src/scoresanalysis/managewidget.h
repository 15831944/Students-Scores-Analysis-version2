#ifndef MANAGEWIDGET_H
#define MANAGEWIDGET_H

#include "login.h"
#include "mainwindow.h"
#include "about.h"

#include <QObject>

class ManageWidget: public QWidget
{
    Q_OBJECT
public:
    explicit ManageWidget(QWidget *parent= 0);
    ~ManageWidget();

private:
    Login *lg;
    MainWindow *w;
    About *about;

public slots:
    void showLogin();
    void showWindow();
    void showAbout();
};
#endif // MANAGEWIDGET_H
