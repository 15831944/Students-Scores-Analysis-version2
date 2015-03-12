#include <QtGui/QApplication>
#include <QTextCodec>
#include <QMessageBox>
#include <QTimer>
#include <QSqlError>
#include <QSplashScreen>

#include "connectdb.h"

#include "login.h"
#include "managewidget.h"
#include "mainwindow.h"
#include "manage_student.h"
#include "manage_course.h"
#include "type_integrate_score.h"
#include "type_score.h"
#include "analysis_group_score.h"
#include "analysis_score.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QTextCodec::setCodecForTr(QTextCodec::codecForName("GB2312"));
    QSplashScreen *splash = new QSplashScreen();
    splash->setPixmap(QPixmap(":/image/start.jpg"));
    QTimer::singleShot(3000, splash, SLOT(close()));
    splash->show();
    Qt::Alignment topRight = Qt::AlignRight | Qt::AlignTop;
    splash->showMessage(QObject::tr("正在加载数据..."),topRight,Qt::yellow);

    QString filename = "./database/stu_analysis.s3db";

    if (!CREATE_SQLITE_CONNECTION(filename))
            return 1;
    splash->showMessage(QObject::tr("正在启动程序..."),topRight,Qt::yellow);

    //Login lo;
    ManageWidget maw;
    //MainWindow mw;

    splash->finish(&maw);
    delete splash;

    //mw.show();
    //lo.show();


    // mw.show();
    /*manage_student w;
    w.show();
    manage_course mc;
    mc.show();
    type_integrate_score ti;
    ti.show();
    type_score tc;
    tc.show();
    //analysis_group_score ag;
    //ag.show();
    analysis_score as;
    as.show();*/

    return a.exec();
}
