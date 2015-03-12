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

    addMinusItemModel->setHeaderData(Add_no, Qt::Horizontal, tr("加减分项目编号"));
    addMinusItemModel->setHeaderData(Add_name, Qt::Horizontal, tr("加减分名称"));
    addMinusItemModel->setHeaderData(Add_classify, Qt::Horizontal, tr("分类"));
    addMinusItemModel->setHeaderData(Add_rank, Qt::Horizontal, tr("级别"));
    addMinusItemModel->setHeaderData(Add_ref_credit, Qt::Horizontal, tr("参考学分"));
    addMinusItemModel->setHeaderData(Add_year, Qt::Horizontal, tr("有效学期"));

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

/* 添加学生个人加分 */
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
    /* 没有找到该学生该学年的transcript成绩单，不能添加返回 */
    if (transcript_id == 0)
    {
        return 2;
    }

    QSqlQuery select;
    select.exec(QString("SELECT * FROM student_add_item "
                        "WHERE item_id = %1 "
                        "AND transcript_id = %2").arg(addno).arg(transcript_id));

    /* 如果已经存在，不能添加 */
    while (select.next()){
        return 1;
    }
    //qDebug() << addno << transcript_id << actualcredit;
    QSqlQuery insert;
    insert.exec(QString("INSERT INTO student_add_item (item_id, transcript_id, actual_credits) "
               "VALUES (%1, %2, %3)").arg(addno).arg(transcript_id).arg(actualcredit));
    return 0;

}

/* 删除学生个人加分 */
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

/* 计算某学你的加分 */
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

/* 计算某学你的减分 */
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
