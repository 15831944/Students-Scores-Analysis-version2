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

    /* �����洢�ɼ��޸��� */
    QStringList modifiedItems;

private slots:
    void updateClasses(const QString &majorname);
    void updateAddMinusItemData();
    void updateStudentIntegrateScoresView();


    /* �޸ķ����Ĳۺ��� */
    void saveItemChanged(QTableWidgetItem* item);
    void canBeModified();
    void saveChanges();

    /* ��ʾ���˼Ӽ��ּ�¼�Ĳۺ��� */
    void updateIndividualAddMinusView();

    /* ���ɾ���Ӽ�����Ŀ�ۺ��� */
    void newAddItem();
    void deleteAddItem();
    void modifyAddItem();
    void saveAddItem();

    /* ���ĳѧ��ĳһѧ��ļӼ��ּ�¼ */
    void addStudentItem();
    void deleteStudentItem();

    void on_pushButton_clicked();
};

#endif // TYPE_INTEGRATE_SCORE_H
