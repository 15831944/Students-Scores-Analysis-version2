#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>

#include "daocourse.h"

CourseDAO::CourseDAO(QObject *parent) : QObject(parent)
{
    courseModel = new QSqlRelationalTableModel(this);
    courseNameModel = new QStringListModel(this);
}

CourseDAO::~CourseDAO()
{
    delete courseModel;
    delete courseNameModel;
}

QSqlRelationalTableModel * CourseDAO::getCourses()
{
    courseModel->setTable("course");

    courseModel->setRelation(7,
            QSqlRelation("school", "school_id", "school_name"));

    /*while (query.next()) {
        QString nclass = query.value(0).toString();
        QString nclass2 = query.value(1).toString();
        qDebug() << nclass << nclass2;
    }*/
    courseModel->setHeaderData(Course_No, Qt::Horizontal, tr("课程号"));
    courseModel->setHeaderData(Course_name, Qt::Horizontal, tr("课程名"));
    courseModel->setHeaderData(Course_school, Qt::Horizontal, tr("开课学院"));
    courseModel->setHeaderData(Course_credit, Qt::Horizontal, tr("学分"));
    courseModel->setHeaderData(Course_attr, Qt::Horizontal, tr("课程属性"));
    courseModel->setHeaderData(Course_grade, Qt::Horizontal, tr("面向年级"));
    courseModel->setHeaderData(Course_considered, Qt::Horizontal, tr("考虑算入平均学分"));

    courseModel->select();

    return courseModel;
}

QStringListModel * CourseDAO::getSemesterCourseNames(int semester)
{

    QSqlQuery query;
    query.exec(QObject::tr("SELECT c.course_name "
               "FROM course c "
               "WHERE c.take_grade ='%1' "
               "AND considerable = 1").arg(semester));

    QStringList strings;
    while (query.next()) {
        QString course = query.value(0).toString();
        strings << course;
        //qDebug() << major;
    }
    courseNameModel->setStringList(strings);
    return courseNameModel;
}

int CourseDAO::saveCourseProvSchool(int schoolid, int courseid, int semester, int considerable)
{
    // qDebug() << schoolid << courseid << semester << considerable;
    QSqlQuery query;
    query.exec(QString("UPDATE course SET school_id = %1, "
               "take_grade = %2, considerable = %3 "
               "WHERE course_id = %4").arg(schoolid).arg(semester).arg(considerable).arg(courseid));
    return 0;
}

