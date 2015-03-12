#include <QStandardItemModel>
#include <QStringListModel>
#include <QSqlQuery>
#include <QDebug>

#include "daogroup.h"


GroupDAO::GroupDAO(QObject *parent) : QObject(parent)
{
    classModel = new QSqlQueryModel(this);
    schoolModel = new QSqlQueryModel(this);
    semesterModel = new QStringListModel(this);
    majorModel = new QSqlQueryModel(this);
    yearModel = new QStringListModel(this);
}


GroupDAO::~GroupDAO()
{
    delete schoolModel;
    delete majorModel;
    delete classModel;
    delete semesterModel;
    delete yearModel;
}

/* ��ûʵ�֣����� */
/*QStandardItemModel * GroupDAO::getSchools()
{
    schoolModel = new QStandardItemModel(this);

    QSqlQuery query;
    query.exec("SELECT DISTINCT s.name FROM school s");

    QVariantList strings;
    while (query.next()) {
        QString school = query.value(0).toString();
        strings << school;
        qDebug() << school;
    }
    schoolModel->setItemData()
    //return schoolModel;
}*/

QSqlQueryModel * GroupDAO::getSchools()
{
    QSqlQuery query;
    query.exec("SELECT DISTINCT s.school_name FROM school s");
    while (query.next()) {
        QString nclass = query.value(0).toString();
        //qDebug() << nclass;
    }
    schoolModel->setQuery(query);
    return schoolModel;
}

QSqlQueryModel * GroupDAO::getMajors(QString schoolname)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT m.major_name "
               "FROM major m "
               "JOIN school s ON s.school_id = m.school_id "
               "WHERE s.school_name ='%1'").arg(schoolname));

    /*QStringList strings;
    while (query.next()) {
        QString major = query.value(0).toString();
        strings << major;
        //qDebug() << major;
    }
    majorModel->setStringList(strings);*/
    majorModel->setQuery(query);
    return majorModel;
}

QSqlQueryModel * GroupDAO::getClasses(QString majorname)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT s.class "
               "FROM student s "
               "JOIN major m ON s.major_id = m.major_id "
               "WHERE m.major_name = '%1'").arg(majorname));
    /*while (query.next()) {
        QString nclass = query.value(0).toString();
        qDebug() << nclass;
    }*/
    classModel->setQuery(query);
    return classModel;
}

/* ���ѧ�� */
QStringListModel * GroupDAO::getSemesters()
{
    QStringList semlist;
    // ������������
    int list;
    QSqlQuery query;
    int studentgrade;
    query.exec(QObject::tr("SELECT DISTINCT s.enroll_year "
               "FROM student s "));
    while (query.next()) {
        studentgrade = query.value(0).toInt();
        // ������������
        list = studentgrade;
        // qDebug() << studentgrade;
    }
    query.exec(QObject::tr("SELECT DISTINCT c.take_grade "
               "FROM course c"));
    int takesem;
    while (query.next()) {
        takesem = query.value(0).toInt();
        // ������������
        semlist << QString::number(list + (takesem - 1) / 2) + QObject::tr("��")
                   + QString::number(list + 1 + (takesem - 1) / 2) + QObject::tr("ѧ���")
                   + QString::number(takesem % 2 + 1) + QObject::tr("ѧ��");
        /*qDebug() << QString::number(list + (takesem - 1) / 2) + QObject::tr("��")
                    + QString::number(list + 1 + (takesem - 1) / 2) + QObject::tr("ѧ���")
                    + QString::number(takesem % 2 + 1) + QObject::tr("ѧ��");*/
    }
    semesterModel->setStringList(semlist);
    return semesterModel;
}

/* �����ַ� */
int GroupDAO::to_take_semester(int enroll_year, QString semesterstr)
{
    QString semesterstr2 = semesterstr;
    semesterstr.truncate(4);
    int enroll = semesterstr.toInt();
    //qDebug() << enroll;
    semesterstr2 = semesterstr2.right(3);
    semesterstr2 = semesterstr2.left(1);
    //qDebug() << semesterstr2;
    //qDebug() << (enroll - enroll_year) * 2 + semesterstr2.toInt();
    return (enroll - enroll_year) * 2 + semesterstr2.toInt();
}

/* �����ַ� */
int GroupDAO::to_valid_year(QString semesterstr)
{
    QString semesterstr2 = semesterstr;
    semesterstr.truncate(4);
    int enroll = semesterstr.toInt();

    return enroll;
}

/* ���ѧԺ */
void GroupDAO::saveSchool(int school_id, QString school_name)
{
    QSqlQuery query;
    query.exec(QObject::tr("SELECT * FROM school s "
                           "WHERE s.school_id = %1").arg(school_id));
    /* ����Ѿ����ڣ�������� */
    if (query.size() != 0)
    {
        //qDebug() << QObject::tr("��school�Ѿ����ڣ��������");
        return;
    }
    else
    {
        //qDebug() << QObject::tr("���school��") << school_name;
        query.exec(QObject::tr("INSERT INTO school "
                               "VALUES(%1,'%2')").arg(school_id).arg(school_name));
        return;
    }
}

QStringListModel * GroupDAO::getYears()
{
    QStringList yearlist;
    QSqlQuery query;
    int studentgrade;
    query.exec(QObject::tr("SELECT DISTINCT s.enroll_year "
               "FROM student s "));
    while (query.next()) {
        studentgrade = query.value(0).toInt();
    }
    for (int i = 0; i < 4; i++)
    {
        yearlist << QString::number(studentgrade + i) + QObject::tr("ѧ��");
    }
    yearModel->setStringList(yearlist);
    return yearModel;
}

/* ���רҵ */
void GroupDAO::saveMajor(QString school_name, int major_id, QString major_name)
{

}

int GroupDAO::findMajorid(QString majorname)
{
    int majorid;
    QSqlQuery query;
    query.exec(QObject::tr("SELECT DISTINCT m.major_id "
               "FROM major m "
               "WHERE m.major_name = '%1'").arg(majorname));
    while (query.next()) {
        majorid = query.value(0).toInt();
    }
    return majorid;
}

int GroupDAO::findSchoolid(QString school)
{
     int schoolid;
     QSqlQuery query;
     query.exec(QObject::tr("SELECT DISTINCT s.school_id "
                "FROM school s "
                "WHERE s.school_name = '%1'").arg(school));
     while (query.next()) {
         schoolid = query.value(0).toInt();
         // qDebug() << schoolid;
     }
     return schoolid;
}


