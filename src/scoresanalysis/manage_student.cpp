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

    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentData(ui->classcombobox->currentText());

    /* ��ʼѡ�в���ģʽΪѧ��ģʽ */
    ui->radioButton->click();

    /* ��ʼ�����޸� */
    cannotModify();

    /* ѧ���༶�б������źŲ� */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentView(const QString &)));

    /* ���ɾ��ѧ���� */
    connect(ui->newButton, SIGNAL(clicked()), this, SLOT(newStudent()));
    connect(ui->deleteButton, SIGNAL(clicked()), this, SLOT(deleteStudent()));

    /* ����ѧ����Ϣ�źŲ� */
    connect(ui->studentView, SIGNAL(clicked(QModelIndex)), this, SLOT(showStudentDetail(const QModelIndex &)));
    connect(ui->stumajorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStuClasses(const QString &)));


    /* �޸�ѧ����Ϣ�źŲ� */
    connect(ui->stunumEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->nameEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));
    connect(ui->sexEdit, SIGNAL(textChanged(QString)), this, SLOT(canSave()));


    connect(ui->firstButton, SIGNAL(clicked()), mapper, SLOT(toFirst()));
    connect(ui->previousButton, SIGNAL(clicked()), mapper, SLOT(toPrevious()));
    connect(ui->nextButton, SIGNAL(clicked()), mapper, SLOT(toNext()));
    connect(ui->lastButton, SIGNAL(clicked()), mapper, SLOT(toLast()));

    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canModify()));

    /* ѡ��ѧ��ѧԺ�༶�źŲ� */
    connect(ui->stuclasscombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));
    connect(ui->stumajorcombobox, SIGNAL(currentIndexChanged(int)), this, SLOT(canSave()));

    /* ����ѧ���� */
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
    /* ����ģ���е�major�к�class�� */
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

    /* ����stuclass��stumajor��ǰѡ�� */
    loadDetailMajorClass();
}

void manage_student::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
    ui->stuclasscombobox->setModel(sgroupDAO->getClasses(majorname));
    ui->stumajorcombobox->setModel(sgroupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));
    /* ��ʼ����ʾ */
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
    /* ����stuclass��stumajor��ǰѡ�� */
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
    /* ���ѧ��ѧ�ţ��������Ա�Ϊ�� */
    if (ui->stunumEdit->text().length() == 0 ||
            ui->nameEdit->text().length() == 0  ||
            ui->sexEdit->text().length() == 0 )
    {
        int result = QMessageBox::warning(this,tr("����"), tr("ѧ��ѧ�ţ��������Ա𣬰༶��רҵ����Ϊ�ա������򲻱��棬�Ƿ������"),
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
    /* ����רҵ��Ϣ */
    int majorid = sgroupDAO->findMajorid(ui->stumajorcombobox->currentText());
    studentDAO->saveStudentMajorClass(ui->stunumEdit->text(),
                                          majorid, ui->stuclasscombobox->currentText());

    QMessageBox::warning(this,tr("��ʾ"), tr("����ɹ���"));
    cannotModify();
    changed = false;
    ui->saveButton->setEnabled(false);
    /* ���������ͼ */
    updateStudentView(ui->classcombobox->currentText());
}

void manage_student::showStudentDetail(const QModelIndex& index)
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
            /* ɾ����ǰ�� */
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

    /* ��������Ϊ���ɱ༭ */
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
    /* ���δѡ���κ���,ָ���һ�� */
    if (row < 0)
    {
        row = ui->studentView->model()->rowCount();
    }
    //qDebug() << row;

    studentModel->insertRows(row, 1);

    /* ָ����ѧ��Ϊĳһ��רҵ��ĳһ��С���� */
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
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��һ�У�"));
        return;
    }
    int r = QMessageBox::warning(this,tr("��ʾ"), tr("����ɾ����ѧ����ȷ��ɾ����"),
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
    majorModel = sgroupDAO->getMajors(QObject::tr("�����΢����ѧԺ"));
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
    /* Ҳ���ܱ��� */
    ui->saveButton->setEnabled(false);
}

void manage_student::canSave()
{
    changed = true;
    ui->saveButton->setEnabled(true);
}

void manage_student::checkFindEdit()
{
    /* ���ֲ���ģʽ */
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
            QMessageBox::warning(this,tr("��ʾ"),
                           tr("�ð༶���Ҳ�����ѧ��: ѧ��") + studentNo, tr("ȷ��"));
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
            QMessageBox::warning(this,tr("��ʾ"),
                           tr("�ð༶���Ҳ�����ѧ��: ����") + studentname, tr("ȷ��"));
            ui->findEdit->clear();
        }
    }

    ui->studentView->horizontalHeader()->setVisible(
            studentModel->rowCount() > 0);
}
