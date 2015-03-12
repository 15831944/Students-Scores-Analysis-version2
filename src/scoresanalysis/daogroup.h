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
    /* ĳѧԺ������רҵģ�� */
    QSqlQueryModel * getMajors(QString schoolname);
    /* ĳרҵ�����а༶ģ�� */
    QSqlQueryModel * getClasses(QString majorname);
    /* ����ѧԺģ�� */
    QSqlQueryModel * getSchools();


    /* ���ѧԺ */
    void saveSchool(int school_id, QString school_name);

    /* ���רҵ */
    void saveMajor(QString school_name, int major_id, QString major_name);

    /* ��ȡ����ѧ�� */
    QStringListModel * getSemesters();

    /* ��ȡ����ѧ�� */
    QStringListModel * getYears();


    /* ��ѧ���ַ�������Ϊ����ѧ�����2009��ѧ���Ͽε�2011-2012ѧ���1ѧ�ڽ���Ϊ5 */
    int to_take_semester(int enroll_year, QString semesterstr);
    /* ��ѧ���ַ�������Ϊ��Ч���,�����2011-2012ѧ���1ѧ�ڽ���Ϊ2011 */
    int to_valid_year(QString semesterstr);

    /* ��ȡmajorid */
    int findMajorid(QString majorname);

    /* ��ȡschoolid */
    int findSchoolid(QString school);
};

#endif // GROUPDAO_H
