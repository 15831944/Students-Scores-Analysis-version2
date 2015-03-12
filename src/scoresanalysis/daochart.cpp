#include "daochart.h"
#include <QDebug>

ChartDAO::ChartDAO(QObject *parent) : QObject(parent)
{
    groupScoreModel = new QStandardItemModel(8, 2, this);
}

ChartDAO::~ChartDAO()
{
    delete groupScoreModel;
}

QAbstractItemModel * ChartDAO::getGroupScores()
{
    QSqlQuery query;

    query.exec(QObject::tr("SELECT c.course_name, c.credits "
                           "FROM course c "
                          ));

    groupScoreModel->removeRows(0, groupScoreModel->rowCount(QModelIndex()), QModelIndex());

    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);

    int row = 0;
    QString group;
    QString num;
    while (query.next() && row < 5)
    {
        groupScoreModel->insertRows(row, 1);
        //qDebug() << row;
        group = query.value(0).toString();
        num = query.value(1).toString();
        //qDebug() << group << num;
        groupScoreModel->setData(groupScoreModel->index(row, 0, QModelIndex()), group);
        groupScoreModel->setData(groupScoreModel->index(row, 1, QModelIndex()), num);
        groupScoreModel->setData(groupScoreModel->index(row, 0, QModelIndex())
                                 , QColor(qrand()%255,qrand()%255,qrand()%255), Qt::DecorationRole);
        //qDebug() << groupScoreModel->data(groupScoreModel->index(row, 0));
        //qDebug() << groupScoreModel->data(groupScoreModel->index(row, 1));

        row++;

    }
    return groupScoreModel;
}

QAbstractItemModel * ChartDAO::getIndividualSemeserScores(QString studentid, int semester)
{

    QSqlQuery query;
    query.exec(QString("SELECT c.course_name, sc.score, sc.exam_date "
                       "FROM student_course sc "
                       "JOIN year_transcript yt ON yt.transcript_id = sc.transcript_id "
                       "JOIN student s ON s.student_id = yt.student_id "
                       "JOIN course c ON sc.course_id = c.course_id "
                       "WHERE c.take_grade = %1 "
                       "AND s.student_id = '%2'").arg(semester).arg(studentid));

    QTime time;
    time = QTime::currentTime();
    qsrand(time.msec() + time.second() * 1000);

    int row = 0;
    QString group;
    QString num;
    while (query.next() && row < 5)
    {
        groupScoreModel->insertRows(row, 1);
        //qDebug() << row;
        group = query.value(0).toString();
        num = query.value(1).toString();
        //qDebug() << group << num;
        groupScoreModel->setData(groupScoreModel->index(row, 0, QModelIndex()), group);
        groupScoreModel->setData(groupScoreModel->index(row, 1, QModelIndex()), num);
        groupScoreModel->setData(groupScoreModel->index(row, 0, QModelIndex())
                                 , QColor(qrand()%255,qrand()%255,qrand()%255), Qt::DecorationRole);
        //qDebug() << groupScoreModel->data(groupScoreModel->index(row, 0));
        //qDebug() << groupScoreModel->data(groupScoreModel->index(row, 1));

        row++;

    }
    return groupScoreModel;
}
