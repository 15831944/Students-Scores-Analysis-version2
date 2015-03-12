#ifndef MANAGE_COURSE_H
#define MANAGE_COURSE_H

#include <QWidget>
#include <QTableView>
#include <QtSql/QSqlRelationalTableModel>
#include <QDataWidgetMapper>
#include <QSqlRecord>
#include "daocourse.h"
#include "daogroup.h"



namespace Ui {
    class manage_course;
}

class manage_course : public QWidget
{
    Q_OBJECT

public:
    explicit manage_course(QWidget *parent = 0);
    ~manage_course();

    void updateCourseView();

private slots:
    void showCourseDetail(const QModelIndex &);
    void newCourse();
    void deleteCourse();

    void on_saveCourse_clicked();

    void canModify();
    void cannotModify();
    void canSave();

private:
    Ui::manage_course *ui;

    CourseDAO * courseDAO;
    GroupDAO * groupDAO;
    QDataWidgetMapper *mapper;
    QSqlTableModel * courseModel;
    int currentRow;

    int changed;

};

#endif // MANAGE_COURSE_H
