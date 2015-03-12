#ifndef DAOADDMINUSITEM_H
#define DAOADDMINUSITEM_H

#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>

enum {
    Add_no = 0,
    Add_name = 3,
    Add_classify = 1,
    Add_rank = 2,
    Add_ref_credit = 4,
    Add_year = 5
};

class AddMinusItemDAO : public QObject
{
    Q_OBJECT

private :

    QSqlRelationalTableModel * addMinusItemModel;

public :
    explicit AddMinusItemDAO(QObject *parent = 0);

    ~AddMinusItemDAO();

    /* ���мӼ�����Ŀģ�� */
    QSqlRelationalTableModel * getAddMinusItems(int semester);

    /* �ԼӼ�����Ŀ����ɾ�Ĳ������Ե��üӼ�����Ŀģ�͵�submitAll()ʵ�� */
    int saveAddItem(QString addname, int year);

    /* ���ѧ�����˼ӷ� */
    int saveStudentAddItem(int addno, QString studentid, int year, double actualcredit);

    /* ɾ��ѧ�����˼ӷ� */
    int deleteStudentAddItem(int addno, QString studentid, int year);

    /* ������˼ӷ� */
    void calculateIndividualAddScore(int year);

    /* ������˼��� */
    void calculateIndividualMinusScore(int year);

};

#endif // DAOADDMINUSITEM_H
