#ifndef ANALYSIS_GROUP_SCORE_H
#define ANALYSIS_GROUP_SCORE_H

#include <QWidget>

#include "daostudent.h"
#include "daocourse.h"
#include "daogroup.h"
#include "daotranscript.h"
#include "daoaddminusitem.h"
#include "daochart.h"

#include "pieview.h"
#include "diagramview.h"

namespace Ui {
    class analysis_group_score;
}

class analysis_group_score : public QWidget
{
    Q_OBJECT

public:
    explicit analysis_group_score(QWidget *parent = 0);
    ~analysis_group_score();

    void loadClassData(QString majorname);
    void loadStudentsSemesterCourseData(QString semestername, QString classname);

private:
    Ui::analysis_group_score *ui;

    GroupDAO * groupDAO;
    CourseDAO * courseDAO;
    TranscriptDAO * transcriptDAO;
    StudentDAO * studentDAO;

    /* Í¼±í */
    ChartDAO * chartDAO;

    QSqlQueryModel * students_score_model;

    /* ±ý×´Í¼ */
    QAbstractItemView * pieleftview;
    QAbstractItemModel * chartLeftModel;
    QAbstractItemView * pierightview;
    QAbstractItemModel * chartRightModel;

private slots:
    void updateClasses(const QString & majorname);
    void updateCourses(const QString & semestername);
    void updateClassScores(const QString & classname);

};

#endif // ANALYSIS_GROUP_SCORE_H
