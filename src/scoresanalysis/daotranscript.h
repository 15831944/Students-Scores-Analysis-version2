#ifndef DAOTRANSCRIPT_H
#define DAOTRANSCRIPT_H

#include <QtSql/QSqlRelationalTableModel>
#include <QSqlQueryModel>
#include <QSqlQuery>
#include <QStringList>

enum {
    ST_Student_id = 0,
    ST_Student_name = 1
};

class TranscriptDAO : public QObject
{
    Q_OBJECT

private :
    QSqlQueryModel * individualTanscriptModel;
    QSqlQueryModel * studentsTanscriptModel;
    //QSqlRelationalTableModel * singleCourseScoresModel;
    QSqlQueryModel * integrateScoresModel;
    QSqlQueryModel * individualAddItemsModel;

public :
    /* 构造函数 */
    explicit TranscriptDAO(QObject *parent = 0);

    /* 析构函数 */
    ~TranscriptDAO();

    /* 得到某学年某班级的所有学生全部课程的成绩单模型 */
    QSqlQueryModel * getStudentSemesterTranscript(int semester, QString classname);

    /* 得到某学年某班级的所有学生某一门课程的单科成绩单模型 */
    QSqlQueryModel * getStudentsSemesterCourseTranscript(int semester, QString classname);

    /* 得到单个学生某学期所有课程的成绩单模型 */
    QSqlQueryModel * getIndividualSemesterCourseTranscript(int semester, QString student_id);

    /* 计算某学你的平均学分积 */
    void calculateIndividualAverScore(int year);

    /* 得到某学年某班级的学生综合测评成绩 */
    QSqlQueryModel * getIntegrateScoresTranscript(int year, QString classname);

    /* 更新或插入某门课的成绩, 即数据库中student_course表插入或修改一行 */
    int saveSingleCourseScore(QString singleCourseScoreItem);

    /* 得到单个学生某学年加减分项目 */
    QSqlQueryModel * getIndividualYearAddItems(int year, QString student_id);

    /* 插入或更新某学生某学年的加减分条目记录, 即数据库中student_add_item表插入或修改一行 */
    int saveSingleAddItem(QString singleAddItem);

    /* 删除某学生某学年的加减分条目记录 */
    int deleteSingleAddItem(QString singleAddItem);

    /* 计算学年综合测评成绩 */
    int calculateIndividualIntegrateScore(int year);

    /* 分割字符串: 把一行"学号_课程名_成绩"的字符分解 */
    bool splitCourseScoreStr(QString courseScoreStr,
                        QString &student_id, QString &course_name, float &score, int &year);
};

#endif // DAOTRANSCRIPT_H
