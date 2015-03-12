#ifndef GROUPDAO_H
#define GROUPDAO_H

#include <QStandardItemModel>
#include <QStringListModel>
#include <QSqlQueryModel>

class GroupDAO : public QObject
{
    Q_OBJECT

private :
    //QStandardItemModel * schoolModel;
    QSqlQueryModel * schoolModel;

    QSqlQueryModel * majorModel;

    QSqlQueryModel * classModel;

    QStringListModel * semesterModel;

    QStringListModel * yearModel;

public :
    explicit GroupDAO(QObject *parent = 0);

    ~GroupDAO();
    //QStandardItemModel * getSchools();
    /* 某学院的所有专业模型 */
    QSqlQueryModel * getMajors(QString schoolname);
    /* 某专业的所有班级模型 */
    QSqlQueryModel * getClasses(QString majorname);
    /* 所有学院模型 */
    QSqlQueryModel * getSchools();


    /* 添加学院 */
    void saveSchool(int school_id, QString school_name);

    /* 添加专业 */
    void saveMajor(QString school_name, int major_id, QString major_name);

    /* 获取所有学期 */
    QStringListModel * getSemesters();

    /* 获取所有学年 */
    QStringListModel * getYears();


    /* 把学期字符串解析为数字学期如把2009级学生上课的2011-2012学年第1学期解析为5 */
    int to_take_semester(int enroll_year, QString semesterstr);
    /* 把学期字符串解析为有效年份,比如把2011-2012学年第1学期解析为2011 */
    int to_valid_year(QString semesterstr);

    /* 获取majorid */
    int findMajorid(QString majorname);

    /* 获取schoolid */
    int findSchoolid(QString school);
};

#endif // GROUPDAO_H
