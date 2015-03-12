#include "type_score.h"
#include "ui_type_score.h"
#include <QDebug>

int type_score::maketable = 0;

type_score::type_score(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::type_score)
{
    ui->setupUi(this);

    studentDAO = new StudentDAO(this);
    groupDAO = new GroupDAO(this);
    courseDAO = new CourseDAO(this);
    transcriptDAO = new TranscriptDAO(this);

    ui->semesteryearcombobox->setModel(groupDAO->getSemesters());
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentsSemesterCourseData(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* ��ʼѡ�в���ģʽ */
    ui->radioButton->click();

    /* ѧ���ɼ�widget����ģʽ */
    makeStudentsScoresTable(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* ѡ��ѧ���źŲ� */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateCourses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClassScores(const QString &)));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClassScores(const QString &)));

    /* �޸ķ����źŲ� */
    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canBeModified()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
    connect(ui->single_score_widget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(saveItemChanged(QTableWidgetItem*)));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelModify()));


    /* ����ѧ���� */
    connect(ui->findEdit, SIGNAL(textChanged(QString)), this, SLOT(checkFindEdit()));
    connect(ui->findButton, SIGNAL(clicked()), this, SLOT(findStudent()));
}

type_score::~type_score()
{
    delete ui;
}

void type_score::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void type_score::loadStudentsSemesterCourseData(QString semestername, QString classname)
{
    int take_semester = groupDAO->to_take_semester(2009 ,semestername);

    /* ��ѧ�ڿγ�combobox���ݼ���, ���ڲ���Ҫ */
    // ui->coursecombobox->setModel(courseDAO->getSemesterCourseNames(take_semester));

    students_score_model = transcriptDAO->getStudentsSemesterCourseTranscript(take_semester, classname);

    ui->single_score_widget->clear();
    makeStudentsScoresTable(semestername, classname);
}

void type_score::updateClasses(const QString & majorname)
{
    groupDAO->getClasses(majorname);
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void type_score::updateCourses(const QString & semestername)
{
    //qDebug() << semestername;
    int take_semester = groupDAO->to_take_semester(2009 ,semestername);
    //qDebug() << take_semester;
    courseDAO->getSemesterCourseNames(take_semester);
}

void type_score::updateClassScores(const QString & classname)
{
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void type_score::makeStudentsScoresTable(QString semestername, QString classname)
{
    /* ȫ�ֱ����������ڼ����ڼ䣬��widgetitem�ĸ��Ĳ����м��� */
    maketable = 0;

    int take_semester = groupDAO->to_take_semester(2009 ,semestername);
    students_score_model = transcriptDAO->getStudentsSemesterCourseTranscript(take_semester, classname);

    int columnCount = students_score_model->columnCount();
    int rowCount = students_score_model->rowCount();

    /* ������� */
    ui->single_score_widget->setColumnCount(columnCount);
    ui->single_score_widget->setRowCount(rowCount);

    //qDebug() << "columnCount:" << columnCount << "rowCount:" << rowCount;

    /* ����ͷ */
    QString coltitlestr;
    for (int i = 0; i <= columnCount - 1; i++)
    {
        coltitlestr = students_score_model->headerData(i, Qt::Horizontal).toString();
        QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
        columntablewidgetitem->setText(coltitlestr);
        ui->single_score_widget->setHorizontalHeaderItem(i, columntablewidgetitem);
    }

    /* ��������� */
    QString itemstr;
    for (int i = 0; i <= rowCount - 1; i++)
    {
        QSqlRecord record = students_score_model->record(i);
        for (int j = 0; j <= columnCount - 1; j++)
        {
            itemstr = record.value(j).toString();
            QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
            tablewidgetitem->setText(itemstr);

            /* �ж��Ƿ�Ϊfloat�������� */
            bool ok;
            float itemscore = itemstr.toFloat(&ok);

            /* ��ǹҿ�ѧ���ɼ�, ����������ɫΪ��ɫ */
            if (ok == true && itemscore < 60)
                tablewidgetitem->setTextColor(QColor(195,0,0));
            ui->single_score_widget->setItem(i ,j, tablewidgetitem);
        }
    }

    /* ��������޸��� */
    modifiedItems.clear();

    /* ��ʼ���ɱ༭,�����޸� */
    ui->single_score_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->single_score_widget->resizeColumnsToContents();
    ui->single_score_widget->horizontalHeader()->setStretchLastSection(true);

    ui->modifyButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    /* ȫ�ֱ�������������� */
    maketable = 1;
}

void type_score::saveItemChanged(QTableWidgetItem* item)
{
    /* ������ڼ���, ��ִ�вۺ��� */
    if (maketable == 0)
    {
        return;
    }

    QString itemstr = item->text();

    QString title = students_score_model->headerData(item->column(), Qt::Horizontal).toString();
    QSqlRecord record = students_score_model->record(item->row());
    QString stuid = record.value(0).toString();

    bool ok;
    float itemscore = itemstr.toFloat(&ok);
    /* �ж��Ƿ��ڳɼ����У��Ƿ�Ϊfloat�������� */
    if (ok == false || item->column() < 2)
    {
        QMessageBox::warning(this,tr("����"), tr("�����޸�") + title + tr("ΪС��"));
        /* �����޸� */
        maketable = 0;
        item->setText(record.value(item->column()).toString());
        maketable = 1;
        return;
    }

    /* ��ǹҿ�ѧ���ɼ�, ����������ɫΪ��ɫ */
    if (ok == true && itemscore < 60)
        item->setTextColor(QColor(195,0,0));
    else if (ok == true)
        item->setTextColor(QColor(0,0,0));

    //qDebug() <<QObject::tr("ѧ��") << stuid << item->row() << item->column() << QObject::tr("��ͷ") << title << ", item:" << itemstr;
    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());
    //qDebug() << stuid + QString("_") + title + QString("_") + itemstr +  QString("_") + QString::number(year);
    modifiedItems << stuid + QString("_") + title + QString("_") + itemstr + QString("_") + QString::number(year);
    ui->saveButton->setEnabled(true);
    ui->cancelButton->setEnabled(true);
}

void type_score::canBeModified()
{
    ui->single_score_widget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->modifyButton->setEnabled(false);
}

void type_score::saveChanges()
{
    if (modifiedItems.size() == 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("û�����κ��޸�"));
    }
    else
    {
        bool modifysuccess = true;
        QString wrongmessage = "";
        QString stritem;
        for (int i = 0; i < modifiedItems.size(); i++)
        {
            wrongmessage += QObject::tr("�����޸�:\n");
            stritem = modifiedItems.at(i);
            int result = transcriptDAO->saveSingleCourseScore(stritem);
            if (result != 0)
            {
                modifysuccess = false;
                wrongmessage += ", " + stritem + "\n";
            }
        }
        if (modifysuccess == false)
            QMessageBox::warning(this,tr("����"), wrongmessage);
        else
            QMessageBox::warning(this,tr("��ʾ"), tr("�޸ĳɹ���"));
    }
    modifiedItems.clear();
    // qDebug() << QObject::tr("modifiedItems�����");
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);

}

void type_score::cancelModify()
{
    modifiedItems.clear();
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}

void type_score::checkFindEdit()
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

void type_score::findStudent()
{
    QString finddes = ui->findEdit->text();
    int findindex = 0;
    if (ui->radioButton_2->isChecked())
    {
        findindex = 1;
    }    int iffind = 0;
    if (findindex == 0) {
        QString studentNo = finddes;
        //qDebug() << studentNo;
        for (int row = 0; row < students_score_model->rowCount(); ++row) {
            QSqlRecord record = students_score_model->record(row);
            if (record.value(Student_No).toString() == studentNo) {
                iffind = 1;
                //qDebug() << row;

                QModelIndex index = students_score_model->index(row, 0);
                ui->single_score_widget->scrollTo(index);
                ui->single_score_widget->selectRow(index.row());

                //ui->single_score_widget->setCurrentIndex(index);

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
        for (int row = 0; row < students_score_model->rowCount(); ++row) {
            QSqlRecord record = students_score_model->record(row);
            if (record.value(Student_name).toString() == studentname) {
                iffind = 1;
                QModelIndex index = students_score_model->index(row, 1);
                ui->single_score_widget->scrollTo(index);
                //ui->single_score_widget->setCurrentIndex(index);
                ui->single_score_widget->selectRow(index.row());

                break;
            }
        }
        if (iffind == 0)
        {
            QMessageBox::warning(this,tr("��ʾѧ���ɼ�"),
                           tr("�ð༶���Ҳ�����ѧ��: ����") + studentname, tr("ȷ��"));
            ui->findEdit->clear();
        }
    }

    ui->single_score_widget->horizontalHeader()->setVisible(
            students_score_model->rowCount() > 0);

}

void type_score::on_countButton_clicked()
{
    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());
    transcriptDAO->calculateIndividualAverScore(year);
    loadStudentsSemesterCourseData(
                ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());
}
