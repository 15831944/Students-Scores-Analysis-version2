#include "manage_student.h"
#include "ui_manage_student.h"

#include <QtSql/QSqlRelationalTableModel>
#include <QtSql/QSqlRelationalDelegate>
#include <QSqlRecord>
#include <QSqlTableModel>

#include "daostudent.h"

#include <QDebug>

manage_student::manage_student(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::manage_student)
{
    ui->setupUi(this);

    studentDAO = new StudentDAO(this);
    groupDAO = new GroupDAO(this);
    sgroupDAO = new GroupDAO(this);
    mapper = new QDataWidgetMapper(this);

    currentRow = 0;
    changed = false;

    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("软件与微电子学院")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentData(ui->classcombobox->currentText());

    /* 开始选中查找模式为学号模式 */
    ui->radioButton->click();

    /* 开始不能修改 */
    cannotModify();

    /* 学生班级列表数据信号槽 */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentView(const QString &)));

    /* 添加删除学生槽 */
    connect(ui->newButton, SIGNAL(clicked()), this, SLOT(newStudent()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteStudent()));

    /* 具体学生信息信号槽 */
    connect(ui->studentView, SIGNAL(clicked(QModelIndex)), this, SLOT(showStudentDetail(const QModelIndex &)));
    connect(ui->stumajorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStuClasses(const QString &)));


    /* 修改学生信息信号槽 */
    connect(ui->stunumEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->sexEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));


    connect(ui->firstButton, SIGNAL(clicked()), mapper, SLOT(toFirst()));
    connect(ui->previousButton, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    connect(ui->nextButton, SIGNAL(clicked()), mapper, SLOT(toNext()));
    connect(ui->lastButton, SIGNAL(clicked()), mapper, SLOT(toLast()));

    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canModify()));

    /* 选择学生学院班级信号槽 */
    connect(ui->stuclasscombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->stumajorcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));

    /* 查找学生槽 */
    connect(ui->findEdit, SIGNAL(textChanged(QString)), this, SLOT(checkFindEdit()));
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findStudent()));
}

manage_student::~manage_student()
{
    delete sgroupDAO;
    delete groupDAO;
    delete studentDAO;
    delete mapper;
    delete ui;
}

void manage_student::loadStudentData(QString classname)
{
    studentModel = studentDAO->getStudents(classname);
    ui->studentView->setModel(studentModel);
    ui->studentView->setItemDelegate(new QSqlRelationalDelegate(this));
    //ui->studentView->setSelectionMode(
    //         QAbstractItemView::SingleSelection);
    ui->studentView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->studentView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->studentView->resizeColumnsToContents();

    ui->studentView->horizontalHeader()->setStretchLastSection(true);
    /* 隐藏模型中的major列和class列 */
    ui->studentView->hideColumn(Student_major);
    ui->studentView->hideColumn(Student_class);
    ui->studentView->hideColumn(Student_total_trans_score);
    ui->studentView->hideColumn(Student_total_trans_order);
    ui->studentView->hideColumn(Student_enroll_year);
    ui->studentView->hideColumn(Student_current_grade);

    //ui->studentView->horizontalHeader()->setVisible(
    //        studentModel->rowCount() > 0);

    mapper->setSubmitPolicy(QDataWidgetMapper::AutoSubmit);
    mapper->setModel(studentDAO->getStudents(classname));
    mapper->setItemDelegate(new QSqlRelationalDelegate(this));
    mapper->addMapping(ui->nameEdit, Student_name);
    mapper->addMapping(ui->sexEdit, Student_sex);
    mapper->addMapping(ui->stunumEdit, Student_No);
    mapper->addMapping(ui->phonenumEdit, Student_phone);
    mapper->addMapping(ui->emailEdit, Student_email);
    mapper->addMapping(ui->qqEdit, Student_QQ);
    mapper->addMapping(ui->dormitoryEdit, Student_domitory);

    mapper->toFirst();

    /* 加载stuclass和stumajor当前选项 */
    loadDetailMajorClass();
}

void manage_student::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
    ui->stuclasscombobox->setModel(sgroupDAO->getClasses(majorname));
    ui->stumajorcombobox->setModel(sgroupDAO->getMajors(QObject::tr("软件与微电子学院")));
    /* 开始不显示 */
    ui->stuclasscombobox->setCurrentIndex(-1);
    ui->stumajorcombobox->setCurrentIndex(-1);
    changed = false;
}

void manage_student::updateClasses(const QString & majorname)
{
    groupDAO->getClasses(majorname);
}

void manage_student::updateStuClasses(const QString & majorname)
{
    sgroupDAO->getClasses(majorname);
}

void manage_student::updateStudentView(const QString & classname)
{
    studentDAO->getStudents(classname);
    mapper->setCurrentIndex(0);

    currentRow = 0;
    /* 加载stuclass和stumajor当前选项 */
    loadDetailMajorClass();
}

void manage_student::on_phonenumEdit_textEdited(QString )
{
    ui->saveButton->setEnabled(true);
    changed = true;
}

void manage_student::on_emailEdit_textEdited(QString )
{
    ui->saveButton->setEnabled(true);
    changed = true;
}

void manage_student::on_qqEdit_textEdited(QString )
{
    ui->saveButton->setEnabled(true);
    changed = true;
}

void manage_student::on_dormitoryEdit_textEdited(QString )
{
    ui->saveButton->setEnabled(true);
    changed = true;
}

void manage_student::on_saveButton_clicked()
{
    /* 如果学生学号，姓名，性别为空 */
    if (ui->stunumEdit->text().length() == 0 ||
            ui->nameEdit->text().length() == 0  ||
            ui->sexEdit->text().length() == 0 )
    {
        int result = QMessageBox::warning(this,tr("警告"), tr("学生学号，姓名，性别，班级，专业不能为空。继续则不保存，是否继续？"),
                                                  QMessageBox::Yes | QMessageBox::No);
            if (result == QMessageBox::No)
                return;
            if (result == QMessageBox::Yes)
            {
                updateStudentView(ui->classcombobox->currentText());
                changed = false;
                return;
            }
    }

    mapper->submit();
    //studentModel->submitAll();
    /* 保存专业信息 */
    int majorid = sgroupDAO->findMajorid(ui->stumajorcombobox->currentText());
    studentDAO->saveStudentMajorClass(ui->stunumEdit->text(),
                                          majorid, ui->stuclasscombobox->currentText());

    QMessageBox::warning(this,tr("提示"), tr("保存成功！"));
    cannotModify();
    changed = false;
    ui->saveButton->setEnabled(false);
    /* 更新左表视图 */
    updateStudentView(ui->classcombobox->currentText());
}

void manage_student::showStudentDetail(const QModelIndex& index)
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
            /* 删除当前行 */
            if (ui->stunumEdit->text().length() == 0 ||
                        ui->nameEdit->text().length() == 0  ||
                        ui->sexEdit->text().length() == 0 )
            {
                updateStudentView(ui->classcombobox->currentText());
                changed = false;
            }
        }
    }

    if (!index.isValid())
        return;

    mapper->setCurrentIndex(index.row());

    currentRow = index.row();

    loadDetailMajorClass();

    /* 重新设置为不可编辑 */
    cannotModify();

    /*QSqlRecord record = studentModel->record(index.row());
    ui->stunumEdit->setText(record.value(Student_No).toString());
    ui->classEdit->setText(record.value(Student_class).toString());
    ui->dormitoryEdit->setText(record.value(Student_domitory).toString());
    ui->sexEdit->setText(record.value(Student_sex).toString());
    ui->emailEdit->setText(record.value(Student_email).toString());
    ui->nameEdit->setText(record.value(Student_name).toString());
    ui->phonenumEdit->setText(record.value(Student_phone).toString());
    ui->qqEdit->setText(record.value(Student_QQ).toString());
    */
}

void manage_student::newStudent()
{
    int row = ui->studentView->currentIndex().row();
    /* 如果未选中任何行,指向第一行 */
    if (row < 0)
    {
        row = ui->studentView->model()->rowCount();
    }
    //qDebug() << row;

    studentModel->insertRows(row, 1);

    /* 指定该学生为某一个专业，某一个小班中 */
    QModelIndex indexClass = studentModel->index(row, Student_class);
    studentModel->setData(indexClass, ui->classcombobox->currentText());
    QModelIndex indexMajor = studentModel->index(row, Student_major);
    studentModel->setData(indexMajor, ui->majorcombobox->currentText());

    QModelIndex index = studentModel->index(row, 0);
    ui->studentView->setCurrentIndex(index);
    //ui->studentView->edit(index);
    ui->stunumEdit->cursor();
    mapper->setCurrentIndex(row);
    canModify();
    changed = false;
}

void manage_student::deleteStudent()
{
    int row = ui->studentView->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("提示"), tr("请先选择一行！"));
        return;
    }
    int r = QMessageBox::warning(this,tr("提示"), tr("您将删除该学生，确定删除？"),
                                 QMessageBox::Yes | QMessageBox::No);
    if (r == QMessageBox::No)
    {
        return;
    }

    studentModel->removeRows(row, 1);
    QModelIndex index = studentModel->index(row - 1, 0);
    ui->studentView->setCurrentIndex(index);
    if (row != 0) {
        mapper->setCurrentIndex(row - 1);
    } else {
        mapper->setCurrentIndex(0);
    }
}

void manage_student::loadDetailMajorClass()
{

    ui->stuclasscombobox->setCurrentIndex(ui->classcombobox->currentIndex());
    ui->stumajorcombobox->setCurrentIndex(ui->majorcombobox->currentIndex());
    changed = false;

    /*

    QSqlRecord record = studentModel->record(currentRow);
    //qDebug() << record.value(Student_No).toString();
    QSqlQueryModel * classModel;

    classModel = sgroupDAO->getClasses(ui->majorcombobox->currentText());
    QModelIndex sindex;
    for (int i = 0; i < classModel->rowCount(); i++)
    {
        sindex = classModel->index(i,0);
        if ((classModel->data(sindex).toString()) == record.value(Student_class).toString())
        {
            ui->stuclasscombobox->setCurrentIndex(i);
            qDebug() << classModel->data(sindex).toString();

        }
    }

    QSqlQueryModel * majorModel;
    majorModel = sgroupDAO->getMajors(QObject::tr("软件与微电子学院"));
    for (int i = 0; i < majorModel->rowCount(); i++)
    {
        sindex = majorModel->index(i,0);
        if ((majorModel->data(sindex).toString()) == record.value(Student_major).toString())
        {
            ui->stumajorcombobox->setCurrentIndex(i);
        }
        //qDebug() << majorModel->data(sindex).toString();
        //qDebug() << record.value(Student_major).toString();
    }*/
}

void manage_student::canModify()
{
    ui->stunumEdit->setEnabled(true);
    ui->nameEdit->setEnabled(true);
    ui->dormitoryEdit->setEnabled(true);
    ui->emailEdit->setEnabled(true);
    ui->phonenumEdit->setEnabled(true);
    ui->qqEdit->setEnabled(true);
    ui->sexEdit->setEnabled(true);
    ui->stuclasscombobox->setEnabled(true);
    ui->stumajorcombobox->setEnabled(true);
}

void manage_student::cannotModify()
{
    ui->stunumEdit->setEnabled(false);
    ui->nameEdit->setEnabled(false);
    ui->dormitoryEdit->setEnabled(false);
    ui->emailEdit->setEnabled(false);
    ui->phonenumEdit->setEnabled(false);
    ui->qqEdit->setEnabled(false);
    ui->sexEdit->setEnabled(false);
    ui->stuclasscombobox->setEnabled(false);
    ui->stumajorcombobox->setEnabled(false);
    /* 也不能保存 */
    ui->saveButton->setEnabled(false);
}

void manage_student::canSave()
{
    changed = true;
    ui->saveButton->setEnabled(true);
}

void manage_student::checkFindEdit()
{
    /* 哪种查找模式 */
    int findindex = 0;
    if (ui->radioButton_2->isChecked())
    {
        findindex = 1;
    }
    if (findindex == 0) {
        QRegExp regExp1("[0-9]{0,10}");
        ui->findEdit->setValidator(new QRegExpValidator(regExp1, this));
    } else if (findindex == 1) {
        ui->findEdit->setValidator(0);
    }
}

void manage_student::findStudent()
{
    QString finddes = ui->findEdit->text();
    int findindex = 0;
    if (ui->radioButton_2->isChecked())
    {
        findindex = 1;
    }    int iffind = 0;
    if (findindex == 0) {
        QString studentNo = finddes;
        qDebug() << studentNo;
        for (int row = 0; row < studentModel->rowCount(); ++row) {
            QSqlRecord record = studentModel->record(row);
            if (record.value(Student_No).toString() == studentNo) {
                iffind = 1;
                qDebug() << row;

                QModelIndex index = ui->studentView->model()->index(row, 0);
                ui->studentView->scrollTo(index);
                ui->studentView->selectRow(index.row());

                break;
            }
        }
        if (iffind == 0)
        {
            QMessageBox::warning(this,tr("提示"),
                           tr("该班级中找不到此学生: 学号") + studentNo, tr("确定"));
            ui->findEdit->clear();
        }
    } else if (findindex == 1){
        QString studentname = finddes;
        for (int row = 0; row < studentModel->rowCount(); ++row) {
            QSqlRecord record = studentModel->record(row);
            if (record.value(Student_name).toString() == studentname) {
                iffind = 1;
                QModelIndex index = ui->studentView->model()->index(row, 1);
                ui->studentView->scrollTo(index);
                ui->studentView->selectRow(index.row());

                break;
            }
        }
        if (iffind == 0)
        {
            QMessageBox::warning(this,tr("提示"),
                           tr("该班级中找不到此学生: 姓名") + studentname, tr("确定"));
            ui->findEdit->clear();
        }
    }

    ui->studentView->horizontalHeader()->setVisible(
            studentModel->rowCount() > 0);
}
