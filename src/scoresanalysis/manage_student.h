#ifndef MANAGE_STUDENT_H
#define MANAGE_STUDENT_H

#include "daostudent.h"
#include "daogroup.h"

#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QMessageBox>


namespace Ui {
    class manage_student;
}

class manage_student : public QWidget
{
    Q_OBJECT

public:
    explicit manage_student(QWidget *parent = 0);
    ~manage_student();

    void loadStudentData(QString classnaem);
    void loadClassData(QString majorname);
    void loadDetailMajorClass();

private:
    Ui::manage_student *ui;

    StudentDAO * studentDAO;
    GroupDAO * groupDAO;

    /* 学生个人处需要用到的另一个指针 */
    GroupDAO * sgroupDAO;

    QSqlTableModel * studentModel;

    QDataWidgetMapper *mapper;

    int currentRow;
    bool changed;

private slots:
    void updateClasses(const QString &majorname);
    void updateStudentView(const QString &classname);

    void showStudentDetail(const QModelIndex &);

    /* 具体学生面板的更改class */
    void updateStuClasses(const QString & majorname);


    void canModify();
    void cannotModify();
    void canSave();

    void newStudent();
    void deleteStudent();

    /* 查找框输入 */
    void checkFindEdit();
    void findStudent();

    void on_phonenumEdit_textEdited(QString );

    void on_emailEdit_textEdited(QString );

    void on_qqEdit_textEdited(QString );

    void on_dormitoryEdit_textEdited(QString );

    void on_saveButton_clicked();
};

#endif // MANAGE_STUDENT_H
