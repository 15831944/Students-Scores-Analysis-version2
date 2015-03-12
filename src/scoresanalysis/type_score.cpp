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
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("软件与微电子学院")));

    loadClassData(ui->majorcombobox->currentText());
    loadStudentsSemesterCourseData(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* 开始选中查找模式 */
    ui->radioButton->click();

    /* 学生成绩widget设置模式 */
    makeStudentsScoresTable(ui->semesteryearcombobox->currentText(), ui->classcombobox->currentText());

    /* 选择学期信号槽 */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateCourses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClassScores(const QString &)));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClassScores(const QString &)));

    /* 修改分数信号槽 */
    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canBeModified()));
    connect(ui->saveButton, SIGNAL(clicked()), this, SLOT(saveChanges()));
    connect(ui->single_score_widget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(saveItemChanged(QTableWidgetItem*)));
    connect(ui->cancelButton, SIGNAL(clicked()), this, SLOT(cancelModify()));


    /* 查找学生槽 */
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

    /* 对学期课程combobox数据加载, 现在不需要 */
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
    /* 全局变量标明处于见表期间，对widgetitem的更改不进行监听 */
    maketable = 0;

    int take_semester = groupDAO->to_take_semester(2009 ,semestername);
    students_score_model = transcriptDAO->getStudentsSemesterCourseTranscript(take_semester, classname);

    int columnCount = students_score_model->columnCount();
    int rowCount = students_score_model->rowCount();

    /* 创建表格 */
    ui->single_score_widget->setColumnCount(columnCount);
    ui->single_score_widget->setRowCount(rowCount);

    //qDebug() << "columnCount:" << columnCount << "rowCount:" << rowCount;

    /* 填充表头 */
    QString coltitlestr;
    for (int i = 0; i <= columnCount - 1; i++)
    {
        coltitlestr = students_score_model->headerData(i, Qt::Horizontal).toString();
        QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
        columntablewidgetitem->setText(coltitlestr);
        ui->single_score_widget->setHorizontalHeaderItem(i, columntablewidgetitem);
    }

    /* 填充表格内容 */
    QString itemstr;
    for (int i = 0; i <= rowCount - 1; i++)
    {
        QSqlRecord record = students_score_model->record(i);
        for (int j = 0; j <= columnCount - 1; j++)
        {
            itemstr = record.value(j).toString();
            QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
            tablewidgetitem->setText(itemstr);

            /* 判断是否为float单精度型 */
            bool ok;
            float itemscore = itemstr.toFloat(&ok);

            /* 标记挂科学生成绩, 设置文字颜色为红色 */
            if (ok == true && itemscore < 60)
                tablewidgetitem->setTextColor(QColor(195,0,0));
            ui->single_score_widget->setItem(i ,j, tablewidgetitem);
        }
    }

    /* 清空已有修改项 */
    modifiedItems.clear();

    /* 初始不可编辑,不可修改 */
    ui->single_score_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->single_score_widget->resizeColumnsToContents();
    ui->single_score_widget->horizontalHeader()->setStretchLastSection(true);

    ui->modifyButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    /* 全局变量标明建表结束 */
    maketable = 1;
}

void type_score::saveItemChanged(QTableWidgetItem* item)
{
    /* 如果正在见表, 不执行槽函数 */
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
    /* 判断是否在成绩项中，是否为float单精度型 */
    if (ok == false || item->column() < 2)
    {
        QMessageBox::warning(this,tr("错误"), tr("不能修改") + title + tr("为小数"));
        /* 不做修改 */
        maketable = 0;
        item->setText(record.value(item->column()).toString());
        maketable = 1;
        return;
    }

    /* 标记挂科学生成绩, 设置文字颜色为红色 */
    if (ok == true && itemscore < 60)
        item->setTextColor(QColor(195,0,0));
    else if (ok == true)
        item->setTextColor(QColor(0,0,0));

    //qDebug() <<QObject::tr("学生") << stuid << item->row() << item->column() << QObject::tr("列头") << title << ", item:" << itemstr;
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
        QMessageBox::warning(this,tr("提示"), tr("没有做任何修改"));
    }
    else
    {
        bool modifysuccess = true;
        QString wrongmessage = "";
        QString stritem;
        for (int i = 0; i < modifiedItems.size(); i++)
        {
            wrongmessage += QObject::tr("不能修改:\n");
            stritem = modifiedItems.at(i);
            int result = transcriptDAO->saveSingleCourseScore(stritem);
            if (result != 0)
            {
                modifysuccess = false;
                wrongmessage += ", " + stritem + "\n";
            }
        }
        if (modifysuccess == false)
            QMessageBox::warning(this,tr("错误"), wrongmessage);
        else
            QMessageBox::warning(this,tr("提示"), tr("修改成功！"));
    }
    modifiedItems.clear();
    // qDebug() << QObject::tr("modifiedItems已清空");
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
            QMessageBox::warning(this,tr("提示"),
                           tr("该班级中找不到此学生: 学号") + studentNo, tr("确定"));
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
            QMessageBox::warning(this,tr("显示学生成绩"),
                           tr("该班级中找不到此学生: 姓名") + studentname, tr("确定"));
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
