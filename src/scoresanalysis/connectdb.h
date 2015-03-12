#ifndef CONNECTDB_H
#define CONNECTDB_H

#include <QSqlDatabase>
#include <QMessageBox>
#include <QDebug>


static bool CREATE_MYSQL_CONNECTION(QString & filename)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");
    db.setDatabaseName("ssd9");
    db.setUserName("root");
    db.setPassword("");

    /*QMySQLDriver mySQLDriver = new

    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName("./student2011NEW.s3db");
    */
    if(!db.open())
    {
        qDebug() << QObject::tr("连接数据库错误");
        QMessageBox::critical(0, QObject::tr("连接数据库错误"), QObject::tr("无法连接到数据库mysql\\ssd9，请检查数据库设置！"), QMessageBox::Ok);
        return false;
    }

    return true;
}

static bool CREATE_SQLITE_CONNECTION(QString &filename)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(filename);

    if(!db.open())
    {
        qDebug() << filename;
        qDebug() << QObject::tr("连接数据库错误");
        QMessageBox::critical(0, QObject::tr("连接数据库错误"),
                              QObject::tr("无法连接到数据库，请检查数据库设置！"), QMessageBox::Ok);
        return false;
    }

    return true;
}

#endif // CONNECTDB_H
