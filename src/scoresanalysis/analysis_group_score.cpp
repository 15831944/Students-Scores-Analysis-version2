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
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentsSemesterCourseData(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* ���Ʊ�״ͼ��ͼ */
    chartLeftModel = chartDAO->getGroupScores();
    pieleftview = new PieView;
    pieleftview->setModel(chartLeftModel);
    ui->gridLayout->addWidget(pieleftview, 0, 0, 0);
    /* �õ�chartModel���һ�е����ִ� */
    QModelIndex chartIndex = chartLeftModel->index(chartLeftModel->rowCount() -1, 1);
    /* ����Ϊ�ַ�, ������ʾ::�б�Ҫ�����򲻳�ͼ���������PieView���dataChanged�ۺ��� */
    chartLeftModel->setData(chartIndex, QString("byebye"));

    /* ���Ʊ�״ͼ��ͼ */
    chartRightModel = chartDAO->getGroupScores();
    //pierightview = new PieView;
    pierightview = new DiagramView(this);
    pierightview->setModel(chartRightModel);
    ui->gridLayout->addWidget(pierightview, 0, 1, 0);
    /* �õ�chartModel���һ�е����ִ� */
    chartIndex = chartRightModel->index(chartRightModel->rowCount() -1, 1);
    /* ����Ϊ�ַ�, ������ʾ::�б�Ҫ�����򲻳�ͼ���������PieView���dataChanged�ۺ��� */
    chartRightModel->setData(chartIndex, QString("byebye"));




    /* ѧ���ɼ�View����ģʽ */
    ui->single_score_view->setModel(students_score_model);
    //ui->single_score_view->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->single_score_view->setSelectionMode(
             QAbstractItemView::SingleSelection);
    ui->single_score_view->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->single_score_view->setEditTriggers(QAbstractItemView::AllEditTriggers);
    //ui->single_score_view->setEditTriggers(QAbstractItemView::EditTriggers);
    ui->single_score_view->resizeColumnsToContents();
    ui->single_score_view->horizontalHeader()->setStretchLastSection(false);

    /* ѡ��ѧ���źŲ� */
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

    /* ��ѧ�ڿγ�combobox���ݼ���, ���ڲ���Ҫ */
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
