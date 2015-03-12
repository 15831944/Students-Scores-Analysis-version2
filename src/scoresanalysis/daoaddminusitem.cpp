#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QDebug>

#include "daoaddminusitem.h"

AddMinusItemDAO::AddMinusItemDAO(QObject *parent) : QObject(parent)
{
    addMinusItemModel = new QSqlRelationalTableModel(this);
}

AddMinusItemDAO::~AddMinusItemDAO()
{
    delete addMinusItemModel;
}

QSqlRelationalTableModel * AddMinusItemDAO::getAddMinusItems(int semester)
{
    addMinusItemModel->setTable("add_minus_item");

    addMinusItemModel->setFilter(QString("add_year = %1").arg(semester));

    addMinusItemModel->setHeaderData(Add_no, Qt::Horizontal, tr("�Ӽ�����Ŀ���"));
    addMinusItemModel->setHeaderData(Add_name, Qt::Horizontal, tr("�Ӽ�������"));
    addMinusItemModel->setHeaderData(Add_classify, Qt::Horizontal, tr("����"));
    addMinusItemModel->setHeaderData(Add_rank, Qt::Horizontal, tr("����"));
    addMinusItemModel->setHeaderData(Add_ref_credit, Qt::Horizontal, tr("�ο�ѧ��"));
    addMinusItemModel->setHeaderData(Add_year, Qt::Horizontal, tr("��Чѧ��"));

    addMinusItemModel->select();

    return addMinusItemModel;
}

int AddMinusItemDAO::saveAddItem(QString addname, int year)
{
    QSqlQuery query;
    query.exec(QString("INSERT INTO add_minus_item (item_name, add_year) "
               "VALUES ('%1', %2)").arg(addname).arg(year));
    return 0;
}

/* ���ѧ�����˼ӷ� */
int AddMinusItemDAO::saveStudentAddItem(int addno, QString studentid, int year, double actualcredit)
{
    //qDebug() << addno << studentid << year << actualcredit;
    int transcript_id = 0;
    QSqlQuery find;
    find.exec(QString("SELECT yt.transcript_id "
              "FROM year_transcript yt "
              "WHERE yt.student_id = %1 "
              "AND yt.transcript_year = %2").arg(studentid).arg(year));
    while (find.next())
    {
        transcript_id = find.value(0).toInt();
    }
    /* û���ҵ���ѧ����ѧ���transcript�ɼ�����������ӷ��� */
    if (transcript_id == 0)
    {
        return 2;
    }

    QSqlQuery select;
    select.exec(QString("SELECT * FROM student_add_item "
                        "WHERE item_id = %1 "
                        "AND transcript_id = %2").arg(addno).arg(transcript_id));

    /* ����Ѿ����ڣ�������� */
    while (select.next()){
        return 1;
    }
    //qDebug() << addno << transcript_id << actualcredit;
    QSqlQuery insert;
    insert.exec(QString("INSERT INTO student_add_item (item_id, transcript_id, actual_credits) "
               "VALUES (%1, %2, %3)").arg(addno).arg(transcript_id).arg(actualcredit));
    return 0;

}

/* ɾ��ѧ�����˼ӷ� */
int AddMinusItemDAO::deleteStudentAddItem(int addno, QString studentid, int year)
{
    int transcript_id = 0;
    QSqlQuery find;
    find.exec(QString("SELECT yt.transcript_id "
              "FROM year_transcript yt "
              "WHERE yt.student_id = %1 "
              "AND yt.transcript_year = %2").arg(studentid).arg(year));
    while (find.next())
    {
        transcript_id = find.value(0).toInt();
    }
    QSqlQuery deletesai;
    deletesai.exec(QString("DELETE FROM student_add_item "
               "WHERE item_id = %1 AND transcript_id = %2").arg(addno).arg(transcript_id));
    return 0;
}

/* ����ĳѧ��ļӷ� */
void AddMinusItemDAO::calculateIndividualAddScore(int year)
{
    QSqlQuery calculate;
    calculate.exec(QString("SELECT (CASE WHEN SUM(sai.actual_credits) > 0 THEN SUM (sai.actual_credits) ELSE 0 END) AS add_score, "
                           "yt.transcript_id, s.student_id, yt.transcript_year "
                           "FROM student_add_item sai "
                           "LEFT JOIN add_minus_item smi ON sai.item_id = smi.item_id "
                           "LEFT JOIN year_transcript yt ON sai.transcript_id = yt.transcript_id "
                           "LEFT JOIN student s ON s.student_id = yt.student_id "
                           "GROUP BY sai.transcript_id "
                           "HAVING yt.transcript_year = %1 "
                           "AND smi.classify = '+';").arg(year));
    QSqlQuery update;
    int transcriptid;
    double averscore;
    while (calculate.next())
    {
        transcriptid = calculate.value(1).toInt();
        averscore = calculate.value(0).toDouble();
        update.exec(QString("UPDATE year_transcript SET add_score = %1 "
                                 "WHERE transcript_id = %2 "
                                 "AND transcript_year = %3")
                         .arg(averscore).arg(transcriptid).arg(year));
    }
}

/* ����ĳѧ��ļ��� */
void AddMinusItemDAO::calculateIndividualMinusScore(int year)
{
    QSqlQuery calculate;
    calculate.exec(QString("SELECT (CASE WHEN SUM(sai.actual_credits) > 0 THEN SUM (sai.actual_credits) ELSE 0 END) AS minus_score, "
                           "yt.transcript_id, s.student_id, yt.transcript_year "
                           "FROM student_add_item sai "
                           "LEFT JOIN add_minus_item smi ON sai.item_id = smi.item_id "
                           "LEFT JOIN year_transcript yt ON sai.transcript_id = yt.transcript_id "
                           "LEFT JOIN student s ON s.student_id = yt.student_id "
                           "GROUP BY sai.transcript_id "
                           "HAVING yt.transcript_year = %1 "
                           "AND smi.classify = '-';").arg(year));
    QSqlQuery update;
    int transcriptid;
    double averscore;
    while (calculate.next())
    {
        transcriptid = calculate.value(1).toInt();
        averscore = calculate.value(0).toDouble();
        update.exec(QString("UPDATE year_transcript SET minus_score = %1 "
                                 "WHERE transcript_id = %2 "
                                 "AND transcript_year = %3")
                         .arg(averscore).arg(transcriptid).arg(year));
    }
}
