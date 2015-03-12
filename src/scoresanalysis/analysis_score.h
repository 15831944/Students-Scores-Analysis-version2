#ifndef ANALYSIS_SCORE_H
#define ANALYSIS_SCORE_H

#include <QWidget>
#include <QSqlRecord>

#include "daotranscript.h"
#include "daogroup.h"
#include "daostudent.h"
#include "daochart.h"

#include "pieview.h"

namespace Ui {
    class analysis_score;
}

class analysis_score : public QWidget
{
    Q_OBJECT

public:
    explicit analysis_score(QWidget *parent = 0);
    ~analysis_score();

    void loadIndividualScoreData(int seme, QString student_id);

    /* 最左界面——加载学生列表数据 */
    void loadStudentData(QString classnaem);
    void loadClassData(QString majorname);

private:
    Ui::analysis_score *ui;

    StudentDAO * studentDAO;
    GroupDAO * groupDAO;
    TranscriptDAO * transcriptDAO;
    ChartDAO * chartDAO;

    /* 饼状图 */
    QAbstractItemView * pieleftview1;
    QAbstractItemModel * chartLeftModel1;
    QAbstractItemView * pierightview1;
    QAbstractItemModel * chartRightModel1;
    /* 饼状图 */
    QAbstractItemView * pieleftview2;
    QAbstractItemModel * chartLeftModel2;
    QAbstractItemView * pierightview2;
    QAbstractItemModel * chartRightModel2;

    QStringListModel * studentListModel;

    QSqlQueryModel * individualModel;

private slots:
    void updateClasses(const QString &majorname);
    void updateStudentView(const QString &classname);

    void updateIndividualScoreView();

};

#endif // ANALYSIS_SCORE_H
