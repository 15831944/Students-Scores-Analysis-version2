#ifndef DAOCHART_H
#define DAOCHART_H

#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QStandardItemModel>
#include <QTime>

class ChartDAO : public QObject
{
    Q_OBJECT

private :

    QAbstractItemModel * groupScoreModel;

public :
    explicit ChartDAO(QObject *parent = 0);

    ~ChartDAO();

    /* ���пγ�ģ�� */
    QAbstractItemModel * getGroupScores();
    QAbstractItemModel * getIndividualSemeserScores(QString studentid, int semester);

};

#endif // DAOCHART_H
