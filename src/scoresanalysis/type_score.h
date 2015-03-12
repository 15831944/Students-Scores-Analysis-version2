#ifndef TYPE_SCORE_H
#define TYPE_SCORE_H

#include <QWidget>
#include <QSqlRecord>
#include <QTableWidgetItem>
#include <QAbstractItemView>
#include <QMessageBox>

#include "daostudent.h"
#include "daocourse.h"
#include "daogroup.h"
#include "daotranscript.h"

namespace Ui {
    class type_score;
}

class type_score : public QWidget
{
    Q_OBJECT

public:
    explicit type_score(QWidget *parent = 0);
    ~type_score();

    void loadClassData(QString majorname);
    void loadStudentsSemesterCourseData(QString semestername, QString classname);

    void makeStudentsScoresTable(QString semestername, QString classname);

private:
    static int maketable;

    Ui::type_score *ui;

    StudentDAO * studentDAO;
    GroupDAO * groupDAO;
    CourseDAO * courseDAO;
    TranscriptDAO * transcriptDAO;

    QSqlQueryModel * students_score_model;

    /* 用作存储成绩修改项 */
    QStringList modifiedItems;

private slots:
    void updateClasses(const QString & majorname);
    void updateCourses(const QString & semestername);
    void updateClassScores(const QString & classname);

    /* 修改分数的槽函数 */
    void saveItemChanged(QTableWidgetItem* item);
    void canBeModified();
    void saveChanges();
    void cancelModify();

    /* 查找框输入 */
    void checkFindEdit();
    void findStudent();

    void on_countButton_clicked();
};

#endif // TYPE_SCORE_H
