#include "daotranscript.h"
#include <QDebug>

TranscriptDAO::TranscriptDAO(QObject *parent) : QObject(parent)
{
    individualTanscriptModel = new QSqlQueryModel();
    studentsTanscriptModel = new QSqlQueryModel();
    //singleCourseScoresModel = new QSqlRelationalTableModel();
    integrateScoresModel = new QSqlQueryModel();
    individualAddItemsModel = new QSqlQueryModel();
}

TranscriptDAO::~TranscriptDAO()
{
    delete individualTanscriptModel;
    delete studentsTanscriptModel;
    //delete singleCourseScoresModel;
    delete integrateScoresModel;
    delete individualAddItemsModel;
}

QSqlQueryModel * TranscriptDAO::getStudentSemesterTranscript(int semester, QString classname)
{
    QSqlQuery query;
    query.exec(QString("SELECT s.student_id, s.name, c.course_name, sc.score "
               "FROM student s "
               "LEFT JOIN student_course sc ON sc.student_id = s.student_id "
               "LEFT JOIN course c ON c.course_id = sc.course_id "
               "WHERE s.class = '%1' "
               "AND c.take_grade = %2").arg(classname).arg(semester));
    individualTanscriptModel->setQuery(query);
    return individualTanscriptModel;
}

/**
 * ���б����sql���
 * SELECT s.student_id, s.student_name, yt.average_score,
 * SUM(CASE WHEN c.course_name = '����1' THEN sc.score ELSE 0 END) AS '����1',
 * SUM(CASE WHEN c.course_name = '����2' THEN sc.score ELSE 0 END) AS '����2'
 * FROM student_course sc
 * JOIN course c ON sc.course_id = c.course_id
 * JOIN year_transcript yt ON yt.transcript_id = sc.transcript_id
 * JOIN student s ON s.student_id = yt.student_id
 * GROUP BY s.student_id;
 */
QSqlQueryModel * TranscriptDAO::getStudentsSemesterCourseTranscript(int semester, QString classname)
{
    //qDebug() << QObject::tr("����getStudentsSemesterCourseTranscript����") << semester << classname;

    /* ��ѯ�ַ� */
    QString sqlstr;
    sqlstr = QString("SELECT s.student_id, s.student_name, ");
    //qDebug() << sqlstr;
    /* ���Ҹ�ѧ�ڵ����пγ� */
    QSqlQuery coursequery;
    coursequery.exec(QObject::tr("SELECT c.course_name "
               "FROM course c "
               "WHERE c.take_grade = %1").arg(semester));

    QStringList strings;
    /* ��һ���Ĵ��� */
    if (coursequery.next())
    {
        QString course = coursequery.value(0).toString();
        strings << course;
        sqlstr += QString("MAX(CASE WHEN c.course_name = '%1' THEN sc.score ELSE 0 END) AS '%1'").arg(course);
        //qDebug() << sqlstr;
    }
    while (coursequery.next()) {
        /* ����֮��Ҫ�Ӷ��� */
        sqlstr += QString(", ");
        QString course = coursequery.value(0).toString();
        strings << course;
        //qDebug() << course;

        /* ��Ӳ�ѯ�ַ��еĿγ̷�������, ��ѯ�������һ�εĳɼ� */
        sqlstr += QString("MAX(CASE WHEN c.course_name = '%1' THEN sc.score ELSE 0 END) AS '%1'").arg(course);
        //qDebug() << sqlstr;
    }
    sqlstr += QString(", yt.average_score ");
    sqlstr += QString("FROM student s "
                      "LEFT JOIN year_transcript yt ON s.student_id = yt.student_id "
                      "LEFT JOIN student_course sc ON yt.transcript_id = sc.transcript_id "
                      "LEFT JOIN course c ON sc.course_id = c.course_id "
                      "WHERE s.class = '%1' "
                      "GROUP BY s.student_id ").arg(classname);

    //qDebug() << sqlstr;

    QSqlQuery query;
    query.exec(sqlstr);
    studentsTanscriptModel->setQuery(query);
    studentsTanscriptModel->setHeaderData(ST_Student_id, Qt::Horizontal, tr("ѧ��"));
    studentsTanscriptModel->setHeaderData(ST_Student_name, Qt::Horizontal, tr("����"));
    int lastColNum = studentsTanscriptModel->columnCount() - 1;
    studentsTanscriptModel->setHeaderData(lastColNum, Qt::Horizontal, tr("ƽ��ѧ�ֻ�"));

    return studentsTanscriptModel;
}

QSqlQueryModel * TranscriptDAO::getIndividualSemesterCourseTranscript(int semester, QString student_id)
{
    //qDebug() << QObject::tr("����getIndividualSemesterCourseTranscript(int semester, QString student_id)����") << semester << student_id;

    QSqlQuery query;
    query.exec(QString("SELECT c.course_name, sc.score, sc.exam_date "
                       "FROM student_course sc "
                       "JOIN year_transcript yt ON yt.transcript_id = sc.transcript_id "
                       "JOIN student s ON s.student_id = yt.student_id "
                       "JOIN course c ON sc.course_id = c.course_id "
                       "WHERE c.take_grade = %1 "
                       "AND s.student_id = '%2'").arg(semester).arg(student_id));
    individualTanscriptModel->setQuery(query);
    individualTanscriptModel->setHeaderData(0, Qt::Horizontal, tr("�γ���"));
    individualTanscriptModel->setHeaderData(1, Qt::Horizontal, tr("����"));
    individualTanscriptModel->setHeaderData(2, Qt::Horizontal, tr("��������"));

    return individualTanscriptModel;
}

QSqlQueryModel * TranscriptDAO::getIntegrateScoresTranscript(int year, QString classname)
{
    //qDebug() << QObject::tr("����getStudentsSemesterCourseTranscript����") << semester << classname;

    /* ��ѯ�ַ� */
    QString sqlstr;
    sqlstr = QString("SELECT s.student_id, s.student_name, yt.average_score, yt.add_score, yt.minus_score, ");
    //qDebug() << sqlstr;
    /* ���Ҹ�ѧ�ڵ����пγ� */
    QSqlQuery extraquery;
    extraquery.exec(QObject::tr("SELECT ei.extra_name "
               "FROM extra_item ei "
               "WHERE ei.valid = %1").arg(year));

    QStringList strings;
    while (extraquery.next()) {
        /* ����֮��Ҫ�Ӷ��� */
        sqlstr += QString(", ");
        QString extra = extraquery.value(0).toString();
        strings << extra;
        //qDebug() << course;

        /* ��Ӳ�ѯ�ַ��еĿγ̷�������, ��ѯ�������һ�εĳɼ� */
        sqlstr += QString("SUM(CASE WHEN ei.extra_name = '%1' THEN ses.extra_s core ELSE 0 END) AS '%1'").arg(extra);
        //qDebug() << sqlstr;
    }
    sqlstr += QString("yt.integrate_score, yt.aver_score_order, yt.integrate_order, yt.is_all_pass, yt.gpa ");
    sqlstr += QString("FROM student s "
                      "LEFT JOIN year_transcript yt ON yt.student_id = s.student_id "
                      "LEFT JOIN student_extra_score ses ON ses.transcript_id = yt.transcript_id "
                      "LEFT JOIN extra_item ei ON ei.extra_id = ses.extra_id "
                      "WHERE yt.transcript_year = %1 "
                      "AND s.class = '%2' "
                      "GROUP BY s.student_id ").arg(year).arg(classname);

    // qDebug() << sqlstr;

    QSqlQuery query;
    query.exec(sqlstr);
    integrateScoresModel->setQuery(query);
    integrateScoresModel->setHeaderData(ST_Student_id, Qt::Horizontal, tr("ѧ��"));
    integrateScoresModel->setHeaderData(ST_Student_name, Qt::Horizontal, tr("����"));
    integrateScoresModel->setHeaderData(2, Qt::Horizontal, tr("ƽ��ѧ�ֻ�"));
    integrateScoresModel->setHeaderData(3, Qt::Horizontal, tr("�ӷ�ѧ��"));
    integrateScoresModel->setHeaderData(4, Qt::Horizontal, tr("����ѧ��"));

    int lastColNum = integrateScoresModel->columnCount() - 1;
    integrateScoresModel->setHeaderData(lastColNum, Qt::Horizontal, tr("GPA"));
    integrateScoresModel->setHeaderData(lastColNum - 1, Qt::Horizontal, tr("�Ƿ�ҿ�"));
    integrateScoresModel->setHeaderData(lastColNum - 2, Qt::Horizontal, tr("�ۺϲ���רҵ����"));
    integrateScoresModel->setHeaderData(lastColNum - 3, Qt::Horizontal, tr("ƽ��ѧ�ֻ�רҵ����"));
    integrateScoresModel->setHeaderData(lastColNum - 4, Qt::Horizontal, tr("�ۺϲ����ɼ�"));


    return integrateScoresModel;
}

/* ����ĳѧ���ƽ��ѧ�ֻ� */
void TranscriptDAO::calculateIndividualAverScore(int year)
{
    QSqlQuery calculate;
    calculate.exec(QString("SELECT (SUM(sc.score * c.credits) / SUM(c.credits)) AS average_score, "
                           "sc.transcript_id, s.student_id, yt.transcript_year "
                           "FROM student_course sc "
                           "LEFT JOIN course c ON sc.course_id = c.course_id "
                           "LEFT JOIN year_transcript yt ON sc.transcript_id = yt.transcript_id "
                           "LEFT JOIN student s ON s.student_id = yt.student_id "
                           "GROUP BY sc.transcript_id "
                           "HAVING yt.transcript_year = %1").arg(year));
    QSqlQuery update;
    int transcriptid;
    double averscore;
    while (calculate.next())
    {
        transcriptid = calculate.value(1).toInt();
        averscore = calculate.value(0).toDouble();
        update.exec(QString("UPDATE year_transcript SET average_score = %1 "
                                 "WHERE transcript_id = %2 "
                                 "AND transcript_year = %3")
                         .arg(averscore).arg(transcriptid).arg(year));
    }
}

QSqlQueryModel * TranscriptDAO::getIndividualYearAddItems(int year, QString student_id)
{
    QSqlQuery query;
    query.exec(QString("SELECT ai.item_id, ai.item_name, sai.actual_credits "
                        "FROM student_add_item sai "
                        "JOIN year_transcript yt ON yt.transcript_id = sai.transcript_id "
                        "JOIN add_minus_item ai ON ai.item_id = sai.item_id "
                        "WHERE yt.student_id = '%1' "
                        "AND yt.transcript_year = %2"
                       ).arg(student_id).arg(year));
    individualAddItemsModel->setQuery(query);
    individualAddItemsModel->setHeaderData(0, Qt::Horizontal, tr("�ӷ�/������Ŀ���"));
    individualAddItemsModel->setHeaderData(1, Qt::Horizontal, tr("�ӷ�/������Ŀ����"));
    individualAddItemsModel->setHeaderData(2, Qt::Horizontal, tr("��ռѧ��"));

    return individualAddItemsModel;
}

int TranscriptDAO::saveSingleAddItem(QString singleAddItem)
{
    qDebug() << QObject::tr("���뺯��saveSingleAddItem(QString singleAddItem)��");
    QString student_id;
    QString item_name;
    QString item_id;
    float actual_credits;
    int transcript_id;
    int year;
    QSqlQuery findAddItems;
    findAddItems.exec(QString("SELECT ad.item_id FROM add_minus_item ai WHERE item_name = '%1'").arg(item_name));
    /* ���������Ŀγ�,�ҳ��γ̺�course_id */
    if (findAddItems.next())
    {
        item_id = findAddItems.value(0).toString();
    }
    else
    {
        return 1;
    }

    /* �õ�year_transcript������transcript_id */
    QSqlQuery findtrans;
    findtrans.exec(QString("SELECT yt.transcript_id FROM year_transcript yt "
                           "WHERE yt.student_id = '%1' AND yt.transcript_year = %2").arg(student_id).arg(year));
    if (findtrans.next())
    {
        transcript_id = findtrans.value(0).toInt();
    }
    else
    {
        /* ����û�н�� */
        /* �������� */
        QSqlQuery inserttrans;
        inserttrans.exec("INSERT INTO year_transcript VALUES()");
    }

    //qDebug() << "course_id:" << course_id << ", transcript_id:" << transcript_id << ", score:" << score << ", is_pass" << is_pass;

    /* �����Ƿ����������һ��������¼ */
    QSqlQuery select;
    select.exec(QString("SELECT * FROM student_add_item sai "
                        "JOIN year_transcript yt "
                        "WHERE sai.item_id = %1 "
                        "AND yt.student_id = '%2'").arg(item_id).arg(student_id));

    /* �����ڼ�¼, ֱ�Ӹ��¼ӷּ���ѧ���� */
    if (select.next())
    {
        QSqlQuery update;
        update.exec(QString("UPDATE student_add_item SET actual_credits = %1 "
                            "WHERE item_id = %2 "
                            "AND transcript_id = %3"
                   ).arg(actual_credits).arg(item_id).arg(transcript_id).arg(year));
    }
    else /* ��������, ������¼ */
    {
        QSqlQuery insert;
        insert.exec(QString("INSERT INTO student_add_item VALUES(%1, %2, %3"
                            ).arg(item_id).arg(transcript_id).arg(actual_credits));
    }
    return 0;
}

int TranscriptDAO::deleteSingleAddItem(QString singleAddItem)
{
    qDebug() << QObject::tr("���뺯��deleteSingleAddItem(QString singleAddItem)��");
    QString student_id;
    QString item_name;
    QString item_id;
    float actual_credits;
    int transcript_id;
    int year;

    QSqlQuery findAddItems;
    findAddItems.exec(QString("SELECT ad.item_id FROM add_minus_item ai WHERE item_name = '%1'").arg(item_name));
    /* ���������Ŀγ�,�ҳ��γ̺�course_id */
    if (findAddItems.next())
    {
        item_id = findAddItems.value(0).toString();
    }
    else
    {
        return 1;
    }
    /* �õ�year_transcript������transcript_id */
    QSqlQuery findtrans;
    findtrans.exec(QString("SELECT yt.transcript_id FROM year_transcript yt "
                           "WHERE yt.student_id = '%1' AND yt.transcript_year = %2").arg(student_id).arg(year));
    if (findtrans.next())
    {
        transcript_id = findtrans.value(0).toInt();
    }
    else
    {
        return 1;
    }

    /* �����Ƿ����������һ��������¼ */
    QSqlQuery select;
    select.exec(QString("SELECT * FROM student_add_item sai "
                        "JOIN year_transcript yt "
                        "WHERE sai.item_id = %1 "
                        "AND yt.student_id = '%2'").arg(item_id).arg(student_id));

    /* �����ڼ�¼, ֱ��ɾ�� */
    if (select.next())
    {
        QSqlQuery deletei;
        deletei.exec(QString("DELETE FROM student_add_item "
                            "WHERE item_id = %2 "
                            "AND transcript_id = %3"
                   ).arg(actual_credits).arg(item_id).arg(transcript_id).arg(year));
    }
    else /* ��������,ֱ�ӷ��س��� */
    {
        return 2;
    }
    return 0;
}

int TranscriptDAO::saveSingleCourseScore(QString singleCourseScoreItem)
{
    // qDebug() << QObject::tr("���뺯��saveSingleCourseScore(QString singleCourseScoreItem)��");
    QString student_id;
    QString course_name;
    QString course_id;
    float score;
    /* ����sqlite���ݿ���û��bool�ͣ���int�Դ��� */
    bool is_pass;
    //int is_pass;
    int transcript_id;
    int year;

    splitCourseScoreStr(singleCourseScoreItem, student_id, course_name, score, year);
    /* �жϷ��������Ƿ�Ϊ������ */
    if (score < 60)
        is_pass = false;
    else
        is_pass = true;

    QSqlQuery findCourse;
    findCourse.exec(QString("SELECT c.course_id FROM course c WHERE course_name = '%1'").arg(course_name));
    /* ���������Ŀγ�,�ҳ��γ̺�course_id */
    if (findCourse.next())
    {
        course_id = findCourse.value(0).toString();
    }
    else
    {
        return 1;
    }
    /* �õ�year_transcript������transcript_id */
    QSqlQuery findtrans;
    findtrans.exec(QString("SELECT yt.transcript_id FROM year_transcript yt "
                           "WHERE yt.student_id = '%1' AND yt.transcript_year = %2").arg(student_id).arg(year));
    if (findtrans.next())
    {
        transcript_id = findtrans.value(0).toInt();
    }
    else
    {
        /* ����û�н�� */
        /* �������� */
        QSqlQuery inserttrans;
        inserttrans.exec(QString("INSERT INTO year_transcript (student_id, transcript_year) VALUES ('%1', %2)").arg(student_id).arg(year));

        /* ���²��� */
        findtrans.exec(QString("SELECT yt.transcript_id FROM year_transcript yt "
                               "WHERE yt.student_id = '%1' AND yt.transcript_year = %2").arg(student_id).arg(year));
        if (findtrans.next())
        {
            transcript_id = findtrans.value(0).toInt();
        }
    }

    // qDebug() << "course_id:" << course_id << ", transcript_id:" << transcript_id << ", score:" << score << ", is_pass" << is_pass;

    /* �����Ƿ����������һ��������¼ */
    QSqlQuery select;
    select.exec(QString("SELECT * FROM student_course sc "
                        "WHERE sc.course_id = %1 "
                        "AND sc.transcript_id = '%2'").arg(course_id).arg(transcript_id));

    /* �����ڼ�¼, ֱ�Ӹ��·��� */
    if (select.next())
    {
        QSqlQuery update;
        update.exec(QString("UPDATE student_course SET score = %1, is_pass = %2 "
                            "WHERE course_id = %3 "
                            "AND transcript_id = %4").arg(score).arg(is_pass).arg(course_id).arg(transcript_id));
    }
    else /* ��������, ������¼ */
    {
        QSqlQuery insert;
        insert.exec(QString("INSERT INTO student_course (course_id, transcript_id, score, exam_date, is_pass) VALUES(%1, %2, %3, "
                            "'%4', %5)").arg(course_id).arg(transcript_id).arg(score).arg("").arg(is_pass));
    }
    return 0;
}


/* ����ѧ���ۺϲ����ɼ� */
int TranscriptDAO::calculateIndividualIntegrateScore(int year)
{
    QSqlQuery calculate;
    calculate.exec(QString("SELECT yt.average_score + "
                           "(CASE WHEN yt.add_score > 0 THEN yt.add_score ELSE 0 END) + "
                           "(CASE WHEN yt.minus_score < 0 THEN yt.minus_score ELSE 0 END) AS integrate_score, "
                           "yt.transcript_id, s.student_id, yt.transcript_year "
                           "FROM year_transcript yt "
                           "LEFT JOIN student s ON s.student_id = yt.student_id "
                           "WHERE yt.transcript_year = %1").arg(year));
    QSqlQuery update;
    int transcriptid;
    double averscore;
    while (calculate.next())
    {
        transcriptid = calculate.value(1).toInt();
        averscore = calculate.value(0).toDouble();
        update.exec(QString("UPDATE year_transcript SET integrate_score = %1 "
                                 "WHERE transcript_id = %2 "
                                 "AND transcript_year = %3")
                         .arg(averscore).arg(transcriptid).arg(year));
    }
    return 0;
}

bool TranscriptDAO::splitCourseScoreStr(QString courseScoreStr, QString &student_id, QString &course_name, float &score, int &year)
{
    QStringList splitstrs = courseScoreStr.split("_");
    bool success;
    student_id = splitstrs.at(0);
    course_name = splitstrs.at(1);
    splitstrs.at(2).toFloat(&success);
    splitstrs.at(3).toInt(&success);
    if (success == false)
    {
        // qDebug() << QObject::tr("splitCourseScoreStr����ȷ");
        return false;
    }
    else
    {
        score = splitstrs.at(2).toFloat();
        year = splitstrs.at(3).toInt();
    }
    // qDebug() << QObject::tr("���뺯��splitCourseScoreStr��") << student_id << course_name << "score:" << score << "year:" << year;
    return true;
}
