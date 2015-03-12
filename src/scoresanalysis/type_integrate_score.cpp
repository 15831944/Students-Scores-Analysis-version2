#include "type_integrate_score.h"
#include "ui_type_integrate_score.h"
#include <QDebug>

int type_integrate_score::maketable = 0;

type_integrate_score::type_integrate_score(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::type_integrate_score)
{
    ui->setupUi(this);

    addMinusItemDAO = new AddMinusItemDAO(this);
    groupDAO = new GroupDAO(this);
    studentDAO = new StudentDAO(this);
    transcriptDAO = new TranscriptDAO(this);

    ui->semesteryearcombobox->setModel(groupDAO->getYears());
    ui->majorcombobox->setModel(groupDAO->getMajors(QObject::tr("�����΢����ѧԺ")));

    loadClassData(ui->majorcombobox->currentText());

    loadAddMinusItemData(groupDAO->to_valid_year(ui->semesteryearcombobox->currentText()));

    loadIntegrateScoreData(groupDAO->to_valid_year(ui->semesteryearcombobox->currentText()),
                           ui->classcombobox->currentText());

    /* ѧ���༶�б������źŲ� */
    connect(ui->majorcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateClasses(const QString &)));
    connect(ui->classcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentIntegrateScoresView()));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateStudentIntegrateScoresView()));
    connect(ui->semesteryearcombobox, SIGNAL(currentIndexChanged(const QString &)), this, SLOT(updateAddMinusItemData()));

    /* �޸��ۺϲ�����ƽ��ѧ�ֻ����ӷּ���������������Ŀ�����źŲ� */
    connect(ui->modifyButton, SIGNAL(clicked()), this, SLOT(canBeModified()));
    connect(ui->integrate_score_widget, SIGNAL(itemChanged(QTableWidgetItem*)), this, SLOT(saveItemChanged(QTableWidgetItem*)));

    /* ѡ��ѧ���ۺϲ����ɼ��б��ĳ���źŲ� */
    connect(ui->integrate_score_widget->selectionModel(),
        SIGNAL(currentRowChanged(const QModelIndex &,
                                         const QModelIndex &)),
        this, SLOT(updateIndividualAddMinusView()));


    /* ���ɾ���ӷּ�����Ŀ�źŲ� */
    connect(ui->newAddButton, SIGNAL(clicked()), this, SLOT(newAddItem()));
    connect(ui->deleteAddButton, SIGNAL(clicked()), this, SLOT(deleteAddItem()));
    connect(ui->modifyAddButton, SIGNAL(clicked()), this, SLOT(modifyAddItem()));
    connect(ui->saveAddButton, SIGNAL(clicked()), this, SLOT(saveAddItem()));

    /* ���ɾ���ӷּ�����Ŀ�źŲ� */
    connect(ui->addStudentItemButton, SIGNAL(clicked()), this, SLOT(addStudentItem()));
    connect(ui->deleteStudentItemButton, SIGNAL(clicked()), this, SLOT(deleteStudentItem()));
}

type_integrate_score::~type_integrate_score()
{
    delete addMinusItemDAO;
    delete groupDAO;
    delete transcriptDAO;
    delete studentDAO;
    delete ui;
}

void type_integrate_score::loadAddMinusItemData(int year)
{
    addMinusItemModel = addMinusItemDAO->getAddMinusItems(year);
    ui->addMinusItemView->setModel(addMinusItemModel);
    ui->addMinusItemView->setItemDelegate(new QSqlRelationalDelegate(this));
    ui->addMinusItemView->setSelectionMode(
             QAbstractItemView::SingleSelection);
    ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addMinusItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->addMinusItemView->verticalHeader()->hide();
    ui->addMinusItemView->resizeColumnsToContents();
    ui->addMinusItemView->horizontalHeader()->setStretchLastSection(true);

    ui->addMinusItemView->hideColumn(Add_year);
}

void type_integrate_score::loadClassData(QString majorname)
{
    ui->classcombobox->setModel(groupDAO->getClasses(majorname));
}

void type_integrate_score::loadIntegrateScoreData(int year, QString classname)
{
    integrateScoresModel = transcriptDAO->getIntegrateScoresTranscript(year, classname);
    makeStudentIntegrateScoresTable(year, classname);
}

void type_integrate_score::updateClasses(const QString & majorname)
{
    groupDAO->getClasses(majorname);
}

void type_integrate_score::updateAddMinusItemData()
{
    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());
    addMinusItemModel = addMinusItemDAO->getAddMinusItems(year);

    /* �����ࣿ */
    ui->addMinusItemView->setModel(addMinusItemModel);
}

void type_integrate_score::updateStudentIntegrateScoresView()
{
    QString classname = ui->classcombobox->currentText();
    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());
    integrateScoresModel = transcriptDAO->getIntegrateScoresTranscript(year, classname);
    makeStudentIntegrateScoresTable(year, classname);

    /* ����������˼Ӽ�����ͼ */
    updateIndividualAddMinusView();
}

void type_integrate_score::makeStudentIntegrateScoresTable(int year, QString classname)
{
    /* ȫ�ֱ����������ڼ����ڼ䣬��widgetitem�ĸ��Ĳ����м��� */
    maketable = 0;

    integrateScoresModel = transcriptDAO->getIntegrateScoresTranscript(year, classname);

    int columnCount = integrateScoresModel->columnCount();
    int rowCount = integrateScoresModel->rowCount();

    /* ������� */
    ui->integrate_score_widget->setColumnCount(columnCount);
    ui->integrate_score_widget->setRowCount(rowCount);

    //qDebug() << "columnCount:" << columnCount << "rowCount:" << rowCount;

    /* ����ͷ */
    QString coltitlestr;
    for (int i = 0; i <= columnCount - 1; i++)
    {
        coltitlestr = integrateScoresModel->headerData(i, Qt::Horizontal).toString();
        QTableWidgetItem * columntablewidgetitem = new QTableWidgetItem();
        columntablewidgetitem->setText(coltitlestr);
        ui->integrate_score_widget->setHorizontalHeaderItem(i, columntablewidgetitem);
    }

    /* ��������� */
    QString itemstr;
    for (int i = 0; i <= rowCount - 1; i++)
    {
        QSqlRecord record = integrateScoresModel->record(i);
        for (int j = 0; j <= columnCount - 1; j++)
        {
            itemstr = record.value(j).toString();
            QTableWidgetItem * tablewidgetitem = new QTableWidgetItem();
            tablewidgetitem->setText(itemstr);

            /* ��ǹҿ�ѧ���ɼ�, ����������ɫΪ��ɫ */
            if (j >= 2)
            {
                /* �ж��Ƿ�Ϊfloat�������� */
                bool ok;
                bool is_all_passed = record.value(columnCount - 2).toBool();
                if (ok && !is_all_passed)
                    tablewidgetitem->setTextColor(QColor(195,0,0));
            }
            ui->integrate_score_widget->setItem(i ,j, tablewidgetitem);
        }
    }

    /* ��������޸��� */
    modifiedItems.clear();

    /* ��ʼ���ɱ༭,�����޸� */
    ui->integrate_score_widget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->modifyButton->setEnabled(true);
    ui->saveButton->setEnabled(false);

    /* ȫ�ֱ�������������� */
    maketable = 1;

    ui->integrate_score_widget->setSelectionBehavior(QAbstractItemView::SelectRows);
}

void type_integrate_score::saveItemChanged(QTableWidgetItem* item)
{
    /* ������ڼ���, ��ִ�вۺ��� */
    if (maketable == 0)
    {
        return;
    }

    /*QString itemstr = item->text();

    QString title = integrateScoresModel->headerData(item->column(), Qt::Horizontal).toString();
    QSqlRecord record = integrateScoresModel->record(item->row());
    QString stuid = record.value(0).toString();

    bool ok;
    float itemscore = itemstr.toFloat(&ok);
    */
    ui->saveButton->setEnabled(true);
}

void type_integrate_score::canBeModified()
{
    ui->integrate_score_widget->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->modifyButton->setEnabled(false);
    ui->integrate_score_widget->setSelectionBehavior(QAbstractItemView::SelectItems);
}

void type_integrate_score::saveChanges()
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
            /* Ҫ�� */
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
    qDebug() << QObject::tr("modifiedItems�����");
    ui->saveButton->setEnabled(false);
    ui->modifyButton->setEnabled(true);

    ui->integrate_score_widget->setSelectionBehavior(QAbstractItemView::SelectItems);

}

void type_integrate_score::updateIndividualAddMinusView()
{
    int seme = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());

    QModelIndex index = ui->integrate_score_widget->currentIndex();
    // qDebug() << QObject::tr("����ۺ���updateIndividualAddMinusView()����") << index.row();
    if (index.isValid()) {
        QSqlRecord record = integrateScoresModel->record(index.row());
        QString student_id = record.value(0).toString();
        QString student_name = record.value(1).toString();
        //qDebug() << QObject::tr("����ۺ���updateIndividualAddMinusView()����:ѧ��:") << seme << student_id;// << student_name;
        ui->stunumEdit->setText(student_id);
        ui->stunameEdit->setText(student_name);
        integrateAddItemModel = transcriptDAO->getIndividualYearAddItems(seme, student_id);

        /* ���������ڹ���ʱ��ǰ���� */
        ui->individual_additem_view->setModel(integrateAddItemModel);
        ui->individual_additem_view->setSelectionMode(
                     QAbstractItemView::SingleSelection);
        ui->individual_additem_view->setSelectionBehavior(QAbstractItemView::SelectRows);        ui->individual_additem_view->hideColumn(0);
        ui->individual_additem_view->resizeColumnsToContents();
        ui->individual_additem_view->horizontalHeader()->setStretchLastSection(true);
    }
}

void type_integrate_score::newAddItem()
{
    int row = ui->addMinusItemView->currentIndex().row();
    /* ���δѡ���κ���,ָ������� */
    if (row < 0)
    {
        row = addMinusItemModel->rowCount();
        if (row < 0)
        {
            row = 0;
        }
    }

    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());
    //qDebug() << year;
    addMinusItemDAO->saveAddItem(QString(""), year);

    updateAddMinusItemData();

    /* ���õķ���������Ϊ����ʹ�����ݿ������������� */
    /*addMinusItemModel->insertRows(row, 1);
    QModelIndex index = addMinusItemModel->index(row, Add_year);
    addMinusItemModel->setData(index, year);*/
//    ui->addMinusItemView->setCurrentIndex(index);

    ui->addMinusItemView->selectRow(row);
    ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->addMinusItemView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->saveAddButton->setEnabled(true);
    ui->modifyAddButton->setEnabled(false);
    //ui->addMinusItemView->edit(index);
}

void type_integrate_score::deleteAddItem()
{
    int row = ui->addMinusItemView->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��һ�У�"));
        return;
    }

    QString itemname = (addMinusItemModel->data(addMinusItemModel->index(row, Add_name))).toString();
    int r =  QMessageBox::warning(this,tr("��ʾ"), tr("ȷ��ɾ��") + itemname + tr("��"),
                                  QMessageBox::Yes |QMessageBox::No);

    if (r == QMessageBox::No)
    {
        QModelIndex index = addMinusItemModel->index(row - 1, 0);
        ui->addMinusItemView->setCurrentIndex(index);
        ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
        ui->addMinusItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
        ui->saveAddButton->setEnabled(false);
        return;
    }

    addMinusItemModel->removeRows(row, 1);
    QModelIndex index = addMinusItemModel->index(row - 1, 0);
    ui->addMinusItemView->setCurrentIndex(index);
    ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addMinusItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->saveAddButton->setEnabled(false);
}

void type_integrate_score::modifyAddItem()
{
    ui->addMinusItemView->setSelectionMode(
             QAbstractItemView::SingleSelection);
    ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectItems);
    ui->addMinusItemView->setEditTriggers(QAbstractItemView::AllEditTriggers);
    ui->saveAddButton->setEnabled(true);
}

void type_integrate_score::saveAddItem()
{
    addMinusItemModel->submitAll();
    ui->addMinusItemView->setSelectionMode(QAbstractItemView::SingleSelection);
    ui->addMinusItemView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->addMinusItemView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->modifyAddButton->setEnabled(true);
    ui->saveAddButton->setEnabled(false);
}

/* ���ĳѧ��ĳһѧ��ļӼ��ּ�¼ */
void type_integrate_score::addStudentItem()
{
    QString studentno = ui->stunumEdit->text();
    if (studentno.length() == 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��һ��ѧ����"));
        return;
    }
    int row = ui->addMinusItemView->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��ѧ��ӷ���Ŀ�е�һ�"));
        return;
    }
    int addno = (addMinusItemModel->data(addMinusItemModel->index(row, Add_no))).toInt();
    double credit = (addMinusItemModel->data(addMinusItemModel->index(row, Add_ref_credit))).toDouble();

    //qDebug() << addno;
    int r = addMinusItemDAO->saveStudentAddItem(addno, studentno, groupDAO->to_valid_year(
                                                    ui->semesteryearcombobox->currentText()), credit);
    //qDebug() << "return:" <<  r;
    if (r != 0)
    {
        QMessageBox::warning(this,tr("����"), tr("��Ӳ��ɹ���"));
    }
    /* ���¸�������ʾ */
    updateIndividualAddMinusView();

}

/* ɾ��ĳѧ��ĳһѧ��ļӼ��ּ�¼ */
void type_integrate_score::deleteStudentItem()
{
    QString studentno = ui->stunumEdit->text();
    if (studentno.length() == 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ��һ��ѧ����"));
        return;
    }
    int row = ui->individual_additem_view->currentIndex().row();
    if (row < 0)
    {
        QMessageBox::warning(this,tr("��ʾ"), tr("����ѡ���ѧ���Ӽ����е�һ����¼��"));
        return;
    }

    int addno = (integrateAddItemModel->data(integrateAddItemModel->index(row, 0))).toInt();

    addMinusItemDAO->deleteStudentAddItem(addno, studentno, groupDAO->to_valid_year(
                                              ui->semesteryearcombobox->currentText()));
    /* ���¸�������ʾ */
    updateIndividualAddMinusView();
}


void type_integrate_score::on_pushButton_clicked()
{
    int year = groupDAO->to_valid_year(ui->semesteryearcombobox->currentText());

    addMinusItemDAO->calculateIndividualAddScore(year);
    addMinusItemDAO->calculateIndividualMinusScore(year);

    transcriptDAO->calculateIndividualIntegrateScore(year);

    updateStudentIntegrateScoresView();
}
