#ifndef TYPE_INTEGRATE_SCORE_H
#define TYPE_INTEGRATE_SCORE_H

#include <QWidget>
#include <QSqlRelationalDelegate>
#include <QSqlRelationalTableModel>
#include <QTableWidgetItem>
#include <QMessageBox>
#include <QSqlRecord>

#include "daoaddminusitem.h"
#include "daogroup.h"
#include "daostudent.h"
#include "daotranscript.h"

namespace Ui {
    class type_integrate_score;
}

class type_integrate_score : public QWidget
{
    Q_OBJECT

public:
    explicit type_integrate_score(QWidget *parent = 0);
    ~type_integrate_score();

    void loadAddMinusItemData(int year);
    void loadClassData(QString majorname);
    void loadIntegrateScoreData(int year, QString classname);

    void makeStudentIntegrateScoresTable(int year, QString classname);

private:

    static int maketable;

    Ui::type_integrate_score *ui;

    AddMinusItemDAO * addMinusItemDAO;
    GroupDAO * groupDAO;
    StudentDAO * studentDAO;
    TranscriptDAO * transcriptDAO;

    QSqlRelationalTableModel * addMinusItemModel;
    QSqlQueryModel * integrateScoresModel;
    QSqlQueryModel * integrateAddItemModel;

    /* 用作存储成绩修改项 */
    QStringList modifiedItems;

private slots:
    void updateClasses(const QString &majorname);
    void updateAddMinusItemData();
    void updateStudentIntegrateScoresView();


    /* 修改分数的槽函数 */
    void saveItemChanged(QTableWidgetItem* item);
    void canBeModified();
    void saveChanges();

    /* 显示个人加减分记录的槽函数 */
    void updateIndividualAddMinusView();

    /* 添加删除加减分项目槽函数 */
    void newAddItem();
    void deleteAddItem();
    void modifyAddItem();
    void saveAddItem();

    /* 添加某学生某一学年的加减分记录 */
    void addStudentItem();
    void deleteStudentItem();

    void on_pushButton_clicked();
};

#endif // TYPE_INTEGRATE_SCORE_H
