#include "analysis_group_score.h"
#include "ui_analysis_group_score.h"

analysis_group_score::analysis_group_score(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::analysis_group_score)
{
    ui->setupUi(this);

    groupDAO = new GroupDAO(this);
    courseDAO = new CourseDAO(this);
    transcriptDAO = new TranscriptDAO(this);
    studentDAO = new StudentDAO(this);
    chartDAO = new ChartDAO(this);

    ui->semesteryearcombobox->setModel(groupDAO->getSemesters());
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("软件与微电子学院")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentsSemesterCourseData(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* 绘制饼状图左图 */
    chartLeftModel = chartDAO->getGroupScores();
    pieleftview = new PieView;
    pieleftview->setModel(chartLeftModel);
    ui->gridLayout->addWidget(pieleftview, 0, 0, 0);
    /* 得到chartModel最后一行的数字处 */
    QModelIndex chartIndex = chartLeftModel->index(chartLeftModel->rowCount() -1, 1);
    /* 设置为字符, 即不显示::有必要！否则不出图，必须调用PieView类的dataChanged槽函数 */
    chartLeftModel->setData(chartIndex, QString("byebye"));

    /* 绘制饼状图右图 */
    chartRightModel = chartDAO->getGroupScores();
    //pierightview = new PieView;
    pierightview = new DiagramView(this);
    pierightview->setModel(chartRightModel);
    ui->gridLayout->addWidget(pierightview, 0, 1, 0);
    /* 得到chartModel最后一行的数字处 */
    chartIndex = chartRightModel->index(chartRightModel->rowCount() -1, 1);
    /* 设置为字符, 即不显示::有必要！否则不出图，必须调用PieView类的dataChanged槽函数 */
    chartRightModel->setData(chartIndex, QString("byebye"));




    /* 学生成绩View设置模式 */
    ui->single_score_view->setModel(students_score_model);
    //ui->single_score_view->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->single_score_view->setSelectionMode(
             QAbstractItemView::SingleSelection);
    ui->single_score_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->single_score_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //ui->single_score_view->setEditTriggers(QAbstractItemView::EditTriggers);
    ui->single_score_view->resizeColumnsToContents();
    ui->single_score_view->horizontalHeader()->setStretchLastSection(false);

    /* 选择学期信号槽 */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateCourses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClassScores(const QString &)));
}

analysis_group_score::~analysis_group_score()
{
    delete groupDAO;
    delete transcriptDAO;
    delete studentDAO;
    delete courseDAO;
    delete ui;
}

void analysis_group_score::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void analysis_group_score::loadStudentsSemesterCourseData(QString semestername, QString classname)
{
    int take_semester = groupDAO->to_take_semester(2009 ,semestername);

    /* 对学期课程combobox数据加载, 现在不需要 */
    // ui->coursecombobox->setModel(courseDAO->getSemesterCourseNames(take_semester));

    students_score_model = transcriptDAO->getStudentsSemesterCourseTranscript(take_semester, classname);
}

void analysis_group_score::updateClasses(const QString & majorname)
{
    groupDAO->getClasses(majorname);
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void analysis_group_score::updateCourses(const QString & semestername)
{
    //qDebug() << semestername;
    int take_semester = groupDAO->to_take_semester(2009 ,semestername);
    //qDebug() << take_semester;
    courseDAO->getSemesterCourseNames(take_semester);
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void analysis_group_score::updateClassScores(const QString & classname)
{
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}
