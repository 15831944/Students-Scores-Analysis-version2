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

    /* ���пγ�ģ�� */
    QSqlRelationalTableModel * getCourses();

    /**
     * ĳһѧ��ѧ�����ϵ����пγ�ģ��,
     * �ڻ���ƽ��ѧ�ֻ����췶Χ��, ��course���е�considerable��ֵΪ0Ϊ1����
     */
    QStringListModel * getSemesterCourseNames(int semester);


    /* �Կγ̵���ɾ�Ĳ������Ե��ÿγ�ģ�͵�submitAll()ʵ�� */
    int saveCourseProvSchool(int schoolid, int courseid, int semester, int considerable);

};

#endif // DAOCOURSE_H
