#include "manage_course.h"
#include "ui_manage_course.h"

#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>

#include "daocourse.h"
#include <QDebug>
#include <QMessageBox>

manage_course::manage_course(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::manage_course)
{
    ui->setupUi(this);


    courseDAO = new CourseDAO(this);
    groupDAO = new GroupDAO(this);

    /* ����γ̵��ڿ�ѧԺ������ѧ��ģ�� */
    ui->schoolcombobox->setModel(groupDAO->getSchools());
    ui->semesteryearcombobox->setModel(groupDAO->getSemesters());

    ui->courseView->setModel(courseDAO->getCourses());
    courseModel=courseDAO->getCourses();
    ui->courseView->setItemDelegate(new QSqlRelationalDelegate(this));
    //ui->courseView->setSelectionMode(
     //        QAbstractItemView::SingleSelection);
    ui->courseView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->courseView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->courseView->resizeColumnsToContents();

    ui->courseView->horizontalHeader()->setStretchLastSection(true);

    //ui->studentView->horizontalHeader()->setVisible(
    //        studentModel->rowCount() > 0);
    currentRow = 0;

    mapper = new QDataWidgetMapper(this);

    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(courseDAO->getCourses());

    mapper->addMapping(ui->lineEdit_courseName, Course_name);
    mapper->addMapping(ui->lineEdit_courseNumber, Course_No);
    mapper->addMapping(ui->lineEdit_courseAtr, Course_attr);
    mapper->addMapping(ui->lineEdit_course_Credit, Course_credit);
    mapper->addMapping(ui->lineEdit_courseScore, 4);

    //mapper->addMapping(ui->schoolcombobox, Course_school);
    //mapper->addMapping(ui->semesteryearcombobox, Course_grade);
    mapper->toFirst();

    /* ��ʼ�����޸ľ�����Ϣ */
    cannotModify();

    changed = false;

    /* ���ɾ���γ̲� */
    connect(ui->newCourse, SIGNAL(clicked()), this, SLOT(newCourse()));
    connect(ui->deleteCourse, SIGNAL(clicked()), this, SLOT(deleteCourse()));

    connect(ui->modifyCourse, SIGNAL(clicked()), this, SLOT(canModify()));

    /* ����γ��źŲ� */
    connect(ui->courseView, SIGNAL(clicked(QModelIndex)), this, SLOT(showCourseDetail(const QModelIndex &)));

    connect(ui->lineEdit_courseAtr, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_courseName, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_courseNumber, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_courseScore, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->lineEdit_course_Credit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->schoolcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->radioButton, SIGNAL(clicked()), this, SLOT(canSave()));
}

manage_course::~manage_course()
{
    delete groupDAO;
    delete courseDAO;
    delete mapper;
    delete ui;
}

void manage_course::updateCourseView()
{
    ui->courseView->setModel(courseDAO->getCourses());
    changed = false;
}

void manage_course::showCourseDetail(const QModelIndex& index)
{
    /* �ж��Ƿ��Ѷ�ĳЩ��Ϣ�޸� */
    if (changed)
    {
        int result = QMessageBox::warning(this,tr("����"), tr("�����޸�ĳЩ��Ϣ���������޸ĵ����ݶ�ʧ������"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            /* ɾ����ʾ�ĵ�ǰ���� */
            if (ui->lineEdit_courseNumber->text().length() == 0 ||
                        ui->lineEdit_courseName->text().length() == 0  ||
                        ui->lineEdit_course_Credit->text().length() == 0 )
            {
                updateCourseView();
                //mapper->toFirst();
                changed = false;
            }
        }
    }


    currentRow = index.row();
    if (!index.isValid())
        return;
    QSqlRecord record = courseModel->record(index.row());
    // qDebug() << record.value(Course_considered).toString();
    /* ��ʾ�Ƿ�������ƽ��ѧ�ֻ� */
    if (record.value(Course_considered).toInt() == 1)
    {
        ui->radioButton->setEnabled(true);
        ui->radioButton->click();
        ui->radioButton->setEnabled(false);
        changed = false;
    }

    /* ��ʾѧԺcombobox */
    QSqlQueryModel * schoolModel;
    schoolModel = groupDAO->getSchools();
    QModelIndex sindex;
    for (int i = 0; i < schoolModel->rowCount(); i++)
    {
        sindex = schoolModel->index(i,0);
        if ((schoolModel->data(sindex).toString()) == record.value(Course_school).toString())
        {
            ui->schoolcombobox->setCurrentIndex(i);
        }
    }

    /* ��ʾѧ��combobox */
    QStringListModel * semesterModel;
    semesterModel = groupDAO->getSemesters();
    for (int i = 0; i < semesterModel->rowCount(); i++)
    {
        sindex = semesterModel->index(i,0);
        /* ��ǰ�꼶Ϊ2009 */
        int grade = groupDAO->to_take_semester(2009, semesterModel->data(sindex, 0).toString());
        if ( grade == record.value(Course_grade).toInt())
        {
            // qDebug() << groupDAO->to_take_semester(2009, semesterModel->data(sindex, 0).toString());
            ui->semesteryearcombobox->setCurrentIndex(i);
        }
    }

    //ui->schoolcombobox->setCurrentIndex(record.value(Course_school).toString());
    //ui->semesteryearcombobox->setItemText(record.value(Course_grade).toString());
    mapper->setCurrentIndex(index.row());

    changed = false;
    cannotModify();
}

void manage_course::newCourse()
{
    int row = ui->courseView->currentIndex().row();
    /* ���δѡ���κ���,ָ������� */
    if (row < 0)
    {
        row = ui->courseView->model()->rowCount();
    }

    courseModel->insertRows(row, 1);

    QModelIndex index = courseModel->index(row, 0);
    ui->courseView->setCurrentIndex(index);
    //ui->courseView->edit(index);
    ui->lineEdit_courseNumber->cursor();
    mapper->setCurrentIndex(row);
    canModify();
    changed = false;
}

void manage_course::deleteCourse()
{
    int row = ui->courseView->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��һ�У�"));
        return;
    }
    int r = QMessageBox::warning(this,tr("��ʾ"), tr("����ɾ���ÿγ̣�ȷ��ɾ����"),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No)
    {
        return;
    }
    courseModel->removeRows(row, 1);
    QModelIndex index = courseModel->index(row - 1, 0);
    ui->courseView->setCurrentIndex(index);
    if (row != 0) {
        mapper->setCurrentIndex(row - 1);
    } else {
        mapper->setCurrentIndex(0);
    }
    courseDAO->getCourses();
    mapper->setCurrentIndex(0);
}

void manage_course::on_saveCourse_clicked()
{


    /* ɾ����ʾ�ĵ�ǰ���� */
    if (ui->lineEdit_courseNumber->text().length() == 0 ||
                ui->lineEdit_courseName->text().length() == 0  ||
                ui->lineEdit_course_Credit->text().length() == 0 )
    {
        int result = QMessageBox::warning(this,tr("����"), tr("�γ̺ţ��γ������γ�ѧ�ֲ���Ϊ�ա������򲻱��棬�Ƿ������"),
                                                  QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::No)
                return;
            if (result == QMessageBox::Yes)
            {
                updateCourseView();
                changed = false;
                return;
            }
    }

    mapper->submit();

    /* ����ѧԺ��Ϣ */
    int schoolid = groupDAO->findSchoolid(ui->schoolcombobox->currentText());
    int semester = groupDAO->to_take_semester(2009, ui->semesteryearcombobox->currentText());
    int considerable = 0;
    if (ui->radioButton->isChecked())
        considerable = 1;
    // qDebug() << schoolid << semester << considerable;
    courseDAO->saveCourseProvSchool(schoolid, ui->lineEdit_courseNumber->text().toInt(), semester, considerable);

    QMessageBox::warning(this,tr("��ʾ"), tr("����ɹ���"));

    cannotModify();
    changed = false;
    ui->saveCourse->setEnabled(false);
    /* ��������ͼ */
    updateCourseView();
}

void manage_course::canModify()
{
    ui->lineEdit_courseName->setEnabled(true);
    ui->lineEdit_courseNumber->setEnabled(true);
    ui->lineEdit_courseAtr->setEnabled(true);
    ui->lineEdit_course_Credit->setEnabled(true);
    ui->radioButton->setEnabled(true);
    ui->schoolcombobox->setEnabled(true);
    ui->semesteryearcombobox->setEnabled(true);
    ui->lineEdit_courseScore->setEnabled(true);
}

void manage_course::cannotModify()
{
    ui->lineEdit_courseName->setEnabled(false);
    ui->lineEdit_courseNumber->setEnabled(false);
    ui->lineEdit_courseAtr->setEnabled(false);
    ui->lineEdit_course_Credit->setEnabled(false);
    ui->radioButton->setEnabled(false);
    ui->schoolcombobox->setEnabled(false);
    ui->semesteryearcombobox->setEnabled(false);
    ui->lineEdit_courseScore->setEnabled(false);
    /* ���ܱ��� */
    ui->saveCourse->setEnabled(false);
}

void manage_course::canSave()
{
    changed = true;
    ui->saveCourse->setEnabled(true);
}
