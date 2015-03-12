#include "daostudent.h"

StudentDAO::StudentDAO(QObject *parent) : QObject(parent)
{
    studentModel = new QSqlRelationalTableModel(this);
    studentListModel = new QStringListModel(this);
}

StudentDAO::~StudentDAO()
{
    delete studentModel;
    delete studentListModel;
}

QSqlRelationalTableModel * StudentDAO::getStudents(QString classname)
{
    studentModel->setTable("student");
    studentModel->setFilter(QString("class = '%1'").arg(classname));

    studentModel->setRelation(Student_major,
            QSqlRelation("major", "major_id", "major_name"));

    studentModel->setSort(Student_No, Qt::AscendingOrder);
    studentModel->setHeaderData(Student_No, Qt::Horizontal, tr("ѧ��"));
    studentModel->setHeaderData(Student_major, Qt::Horizontal, tr("רҵ"));
    studentModel->setHeaderData(Student_name, Qt::Horizontal, tr("����"));
    studentModel->setHeaderData(Student_sex, Qt::Horizontal, tr("�Ա�"));
    studentModel->setHeaderData(Student_domitory, Qt::Horizontal, tr("����"));

    studentModel->setHeaderData(Student_enroll_year, Qt::Horizontal, tr("��ѧ�꼶"));
    studentModel->setHeaderData(Student_current_grade, Qt::Horizontal, tr("��ǰ�꼶"));

    studentModel->setHeaderData(Student_class, Qt::Horizontal, tr("�༶"));
    studentModel->setHeaderData(Student_total_trans_score, Qt::Horizontal, tr("��ƽ��ѧ��"));
    studentModel->setHeaderData(Student_total_trans_order, Qt::Horizontal, tr("��ƽ��ѧ������"));
    studentModel->setHeaderData(Student_QQ, Qt::Horizontal, tr("QQ"));
    studentModel->setHeaderData(Student_email, Qt::Horizontal, tr("����"));
    studentModel->setHeaderData(Student_phone, Qt::Horizontal, tr("�ֻ�"));

    studentModel->select();

    return studentModel;
}

QStringListModel * StudentDAO::getStudentList(QString classname)
{
    QSqlQuery query;
    query.exec(QString("SELECT s.student_id, s.student_name "
               "FROM student s "
               "WHERE s.class = '%1'").arg(classname));
    QStringList strings;
    while (query.next()) {
        QString student_id = query.value(0).toString();
        QString student_name = query.value(1).toString();
        strings << student_id + QString("  ") + student_name;
    }
    studentListModel->setStringList(strings);

    return studentListModel;
}

/* int iΪ0����ѧ�ţ�iΪ1�������� */
QString StudentDAO::to_stuid_or_name(QString stulistitem, int i)
{
    QStringList stu_id_name = stulistitem.split("  ");
    return stu_id_name.at(i);
}

int StudentDAO::saveStudentMajorClass(QString stunum, int majorid, QString classid)
{
    QSqlQuery query;
    query.exec(QString("UPDATE student SET major_id = %1, class = '%2' , enroll_year = 2009 "
               "WHERE student_id = '%3'").arg(majorid).arg(classid).arg(stunum));
    return 0;
}


