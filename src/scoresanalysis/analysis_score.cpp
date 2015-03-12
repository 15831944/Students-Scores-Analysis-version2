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

    /* �������ݼ��� */
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));
    ui->student_list_view->setEditTriggers(QAbstractItemView::NoEditTriggers);
    /* �������ݼ��� */
    ui->semesteryearcombobox->setModel(groupDAO->getSemesters());

    loadClassData(ui->majorcombobox->currentText());
    loadStudentData(ui->classcombobox->currentText());

    /* ��ʼ���ݼ��أ�δ��� */
    /* ����ѧ��ĳѧ�ڿγ����ݣ��õ�������ʱ����semestr��student_id����������semester����ֵ */
    /*QString semestr = ui->semesteryearcombobox->currentText();
    QModelIndex index = ui->student_list_view->;
    QString student_id = studentListModel->data(index, 0).toString();
    QString student_name = studentDAO->to_stuid_or_name(student_id, 1);
    student_id = studentDAO->to_stuid_or_name(student_id, 0);
    ui->stunumEdit->setText(student_id);
    ui->stunameEdit->setText(student_name);
    loadIndividualScoreData(groupDAO->to_take_semester(2009, semestr), student_id);
    */

    /* ѧ���༶�б������źŲ� */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentView(const QString &)));

    /* ѧ�����˸��Ƴɼ��źŲ� */
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
    /* �д� */
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

    /* �õ�chartModel���һ�е����ִ� */
    QModelIndex chartIndex = chartLeftModel1->index(chartLeftModel1->rowCount() -1, 1);
    /* ����Ϊ�ַ�, ������ʾ::�б�Ҫ�����򲻳�ͼ���������PieView���dataChanged�ۺ��� */
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
        //qDebug() << QObject::tr("updateIndividualScoreView()����:ѧ��:") << seme << student_id << student_name;
        ui->stunumEdit->setText(student_id);
        ui->stunameEdit->setText(student_name);
        individualModel = transcriptDAO->getIndividualSemesterCourseTranscript(seme, student_id);

        /* ���������ڹ���ʱ��ǰ���� */
        ui->individual_score_view->setModel(individualModel);
    }
}
