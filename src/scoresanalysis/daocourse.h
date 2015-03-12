#ifndef DAOCOURSE_H
#define DAOCOURSE_H

#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QStringListModel>

enum {
    Course_No = 0,
    Course_name = 1,
    Course_school = 7,
    Course_credit = 2,
    Course_attr = 3,
    Course_grade = 5,
    Course_considered = 6,
};

class CourseDAO : public QObject
{
    Q_OBJECT

private :

    QSqlRelationalTableModel * courseModel;
    QStringListModel * courseNameModel;

public :
    explicit CourseDAO(QObject *parent = 0);

    ~CourseDAO();

    /* 所有课程模型 */
    QSqlRelationalTableModel * getCourses();

    /**
     * 某一学期学生所上的所有课程模型,
     * 在或不在平均学分积考察范围内, 即course表中的considerable列值为0为1均可
     */
    QStringListModel * getSemesterCourseNames(int semester);


    /* 对课程的增删改操作可以调用课程模型的submitAll()实现 */
    int saveCourseProvSchool(int schoolid, int courseid, int semester, int considerable);

};

#endif // DAOCOURSE_H
