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

    /* 具体课程的授课学院，面向学期模型 */
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

    /* 初始不能修改具体信息 */
    cannotModify();

    changed = false;

    /* 添加删除课程槽 */
    connect(ui->newCourse, SIGNAL(clicked()), this, SLOT(newCourse()));
    connect(ui->deleteCourse, SIGNAL(clicked()), this, SLOT(deleteCourse()));

    connect(ui->modifyCourse, SIGNAL(clicked()), this, SLOT(canModify()));

    /* 具体课程信号槽 */
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
    /* 判断是否已对某些信息修改 */
    if (changed)
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("您已修改某些信息，继续则修改的内容丢失！！！"),
                                          QMessageBox::Yes | QMessageBox::No);
        if (result == QMessageBox::No)
            return;
        else if (result == QMessageBox::Yes)
        {
            /* 删除显示的当前空行 */
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
    /* 显示是否考虑算入平均学分积 */
    if (record.value(Course_considered).toInt() == 1)
    {
        ui->radioButton->setEnabled(true);
        ui->radioButton->click();
        ui->radioButton->setEnabled(false);
        changed = false;
    }

    /* 显示学院combobox */
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

    /* 显示学期combobox */
    QStringListModel * semesterModel;
    semesterModel = groupDAO->getSemesters();
    for (int i = 0; i < semesterModel->rowCount(); i++)
    {
        sindex = semesterModel->index(i,0);
        /* 当前年级为2009 */
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
    /* 如果未选中任何行,指向最后行 */
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
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    int r = QMessageBox::warning(this,tr("提示"), tr("您将删除该课程，确定删除？"),
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


    /* 删除显示的当前空行 */
    if (ui->lineEdit_courseNumber->text().length() == 0 ||
                ui->lineEdit_courseName->text().length() == 0  ||
                ui->lineEdit_course_Credit->text().length() == 0 )
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("课程号，课程名，课程学分不能为空。继续则不保存，是否继续？"),
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

    /* 保存学院信息 */
    int schoolid = groupDAO->findSchoolid(ui->schoolcombobox->currentText());
    int semester = groupDAO->to_take_semester(2009, ui->semesteryearcombobox->currentText());
    int considerable = 0;
    if (ui->radioButton->isChecked())
        considerable = 1;
    // qDebug() << schoolid << semester << considerable;
    courseDAO->saveCourseProvSchool(schoolid, ui->lineEdit_courseNumber->text().toInt(), semester, considerable);

    QMessageBox::warning(this,tr("提示"), tr("保存成功！"));

    cannotModify();
    changed = false;
    ui->saveCourse->setEnabled(false);
    /* 更新左视图 */
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
    /* 不能保存 */
    ui->saveCourse->setEnabled(false);
}

void manage_course::canSave()
{
    changed = true;
    ui->saveCourse->setEnabled(true);
}
