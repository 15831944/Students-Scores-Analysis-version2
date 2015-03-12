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

    /* 所有加减分项目模型 */
    QSqlRelationalTableModel * getAddMinusItems(int semester);

    /* 对加减分项目的增删改操作可以调用加减分项目模型的submitAll()实现 */
    int saveAddItem(QString addname, int year);

    /* 添加学生个人加分 */
    int saveStudentAddItem(int addno, QString studentid, int year, double actualcredit);

    /* 删除学生个人加分 */
    int deleteStudentAddItem(int addno, QString studentid, int year);

    /* 计算个人加分 */
    void calculateIndividualAddScore(int year);

    /* 计算个人减分 */
    void calculateIndividualMinusScore(int year);

};

#endif // DAOADDMINUSITEM_H
