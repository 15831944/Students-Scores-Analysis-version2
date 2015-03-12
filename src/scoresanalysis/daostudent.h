#ifndef STUDENTDAO_H
#define STUDENTDAO_H

#include <QtSql/QSqlRelationalTableModel>
#include <QStringListModel>
#include <QSqlQuery>

enum {
    Student_No = 0,
    Student_major = 1,
    Student_name = 2,
    Student_sex = 3,
    Student_domitory = 4,
    Student_enroll_year = 5,
    Student_current_grade = 6,
    Student_class = 7,
    Student_total_trans_score = 8,
    Student_total_trans_order = 9,
    Student_QQ = 10,
    Student_email = 11,
    Student_phone = 12,
};

class StudentDAO : public QObject
{
    Q_OBJECT

private :
    QSqlRelationalTableModel * studentModel;
    QStringListModel * studentListModel;

public :
    explicit StudentDAO(QObject *parent = 0);

    ~StudentDAO();
    /* �õ�����ѧ��ģ�� */
    QSqlRelationalTableModel * getStudents(QString classname);
    /* �õ�ѧ���б�ģ�ͣ�ֻ��ʾ"student_id"_"student_name" */
    QStringListModel * getStudentList(QString classname);

    /* ���б��е�һ���ַ����ֽ�i=1ȡ����, i=0ȡѧ�� */
    QString to_stuid_or_name(QString stulistitem, int i);
    /* ��ѧ��������Ϣ����ɾ�Ĳ������Ե���ѧ��ģ�͵�submitAll()ʵ�� */

    int saveStudentMajorClass(QString stunum, int majorid, QString classid);
};

#endif // STUDENTDAO_H
