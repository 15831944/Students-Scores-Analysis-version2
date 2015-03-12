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
    /* ���캯�� */
    explicit TranscriptDAO(QObject *parent = 0);

    /* �������� */
    ~TranscriptDAO();

    /* �õ�ĳѧ��ĳ�༶������ѧ��ȫ���γ̵ĳɼ���ģ�� */
    QSqlQueryModel * getStudentSemesterTranscript(int semester, QString classname);

    /* �õ�ĳѧ��ĳ�༶������ѧ��ĳһ�ſγ̵ĵ��Ƴɼ���ģ�� */
    QSqlQueryModel * getStudentsSemesterCourseTranscript(int semester, QString classname);

    /* �õ�����ѧ��ĳѧ�����пγ̵ĳɼ���ģ�� */
    QSqlQueryModel * getIndividualSemesterCourseTranscript(int semester, QString student_id);

    /* ����ĳѧ���ƽ��ѧ�ֻ� */
    void calculateIndividualAverScore(int year);

    /* �õ�ĳѧ��ĳ�༶��ѧ���ۺϲ����ɼ� */
    QSqlQueryModel * getIntegrateScoresTranscript(int year, QString classname);

    /* ���»����ĳ�ſεĳɼ�, �����ݿ���student_course�������޸�һ�� */
    int saveSingleCourseScore(QString singleCourseScoreItem);

    /* �õ�����ѧ��ĳѧ��Ӽ�����Ŀ */
    QSqlQueryModel * getIndividualYearAddItems(int year, QString student_id);

    /* ��������ĳѧ��ĳѧ��ļӼ�����Ŀ��¼, �����ݿ���student_add_item�������޸�һ�� */
    int saveSingleAddItem(QString singleAddItem);

    /* ɾ��ĳѧ��ĳѧ��ļӼ�����Ŀ��¼ */
    int deleteSingleAddItem(QString singleAddItem);

    /* ����ѧ���ۺϲ����ɼ� */
    int calculateIndividualIntegrateScore(int year);

    /* �ָ��ַ���: ��һ��"ѧ��_�γ���_�ɼ�"���ַ��ֽ� */
    bool splitCourseScoreStr(QString courseScoreStr,
                        QString &student_id, QString &course_name, float &score, int &year);
};

#endif // DAOTRANSCRIPT_H
