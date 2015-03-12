#include "analysis_score.h"
#include "ui_analysis_score.h"

#include <QDebug>

analysis_score::analysis_score(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::analysis_score)
{
    ui->setupUi(this);

    studentDAO = new StudentDAO(this);
    groupDAO = new GroupDAO(this);
    transcriptDAO = new TranscriptDAO(this);
    chartDAO = new ChartDAO(this);

    /* 左栏数据加载 */
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("软件与微电子学院")));
    ui->student_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /* 中栏数据加载 */
    ui->semesteryearcombobox->setModel(groupDAO->getSemesters());

    loadClassData(ui->majorcombobox->currentText());
    loadStudentData(ui->classcombobox->currentText());

    /* 初始数据加载，未解决 */
    /* 加载学生某学期课程数据，用到两个临时变量semestr和student_id，用来计算semester数字值 */
    /*QString semestr = ui->semesteryearcombobox->currentText();
    QModelIndex index = ui->student_list_view->;
    QString student_id = studentListModel->data(index, 0).toString();
    QString student_name = studentDAO->to_stuid_or_name(student_id, 1);
    student_id = studentDAO->to_stuid_or_name(student_id, 0);
    ui->stunumEdit->setText(student_id);
    ui->stunameEdit->setText(student_name);
    loadIndividualScoreData(groupDAO->to_take_semester(2009, semestr), student_id);
    */

    /* 学生班级列表数据信号槽 */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentView(const QString &)));

    /* 学生个人各科成绩信号槽 */
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateIndividualScoreView()));
    connect(ui->student_list_view->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex &,
                                         const QModelIndex &)),
        this, SLOT(updateIndividualScoreView()));

}

analysis_score::~analysis_score()
{
    delete studentDAO;
    delete transcriptDAO;
    delete groupDAO;
    delete ui;
}

void analysis_score::loadStudentData(QString classname)
{
    studentListModel = studentDAO->getStudentList(classname);
    ui->student_list_view->setModel(studentListModel);
}

void analysis_score::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
}

void analysis_score::updateClasses(const QString & majorname)
{
    groupDAO->getClasses(majorname);
}

void analysis_score::updateStudentView(const QString & classname)
{
    studentDAO->getStudentList(classname);
    /* 有错 */
    //ui->student_list_view->setCurrentIndex(individualModel->index(0, 0));
}

void analysis_score::loadIndividualScoreData(int seme, QString student_id)
{
    individualModel = transcriptDAO->getIndividualSemesterCourseTranscript(seme, student_id);
    ui->individual_score_view->setModel(individualModel);

    //chartLeftModel1 = chartDAO->getIndividualSemeserScores(student_id, seme);

    chartLeftModel1 = chartDAO->getGroupScores();
    pieleftview1 = new PieView;
    pieleftview1->setModel(chartLeftModel1);

    /* 得到chartModel最后一行的数字处 */
    QModelIndex chartIndex = chartLeftModel1->index(chartLeftModel1->rowCount() -1, 1);
    /* 设置为字符, 即不显示::有必要！否则不出图，必须调用PieView类的dataChanged槽函数 */
    chartLeftModel1->setData(chartIndex, QString("byebye"));

    ui->gridLayout_3->addWidget(pieleftview1, 0, 0, 0);

    ui->gridLayout_3->addWidget(pieleftview1, 0, 2, 0);
    ui->gridLayout_3->addWidget(pieleftview1, 2, 0, 0);
    ui->gridLayout_3->addWidget(pieleftview1, 2, 2, 0);

}

void analysis_score::updateIndividualScoreView()
{
    int seme = groupDAO->to_take_semester(2009, ui->semesteryearcombobox->currentText());

    QModelIndex index = ui->student_list_view->currentIndex();
    if (index.isValid()) {
        QString student_id = studentListModel->data(index, 0).toString();
        QString student_name = studentDAO->to_stuid_or_name(student_id, 1);
        student_id = studentDAO->to_stuid_or_name(student_id, 0);
        //qDebug() << QObject::tr("updateIndividualScoreView()方法:学期:") << seme << student_id << student_name;
        ui->stunumEdit->setText(student_id);
        ui->stunameEdit->setText(student_name);
        individualModel = transcriptDAO->getIndividualSemesterCourseTranscript(seme, student_id);

        /* 不合理，该在构造时提前设置 */
        ui->individual_score_view->setModel(individualModel);
    }
}
