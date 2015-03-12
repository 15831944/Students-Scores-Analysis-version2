#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "QBoxLayout"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->gridLayout->addWidget(ui->page, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page_2, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page_3, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page_4, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page_5, 0, 1, 1, 1);
    ui->gridLayout->addWidget(ui->page_6, 0, 1, 1, 1);
    ui->page->setVisible(true);
//    manage_student *man_student = new manage_student(ui->tab);
//    manage_course *man_course = new manage_course(ui->tab_2);
//    type_integrate_sore *ti_score = new type_integrate_score(ui->tab_3);
//    type_score *t_score = new type_score(ui->tab_4);
//    analysis_group_score *anal_gscore = new analysis_group_score(ui->tab_5);
//    analysis_score *anal_score = new analysis_score(ui->tab_6);

    manage_student *man_student = new manage_student(ui->page);
    manage_course *man_course = new manage_course(ui->page_2);
    type_integrate_score *ti_score = new type_integrate_score(ui->page_3);
    type_score *t_score = new type_score(ui->page_4);
    analysis_group_score *anal_gscore = new analysis_group_score(ui->page_5);
    analysis_score *anal_score = new analysis_score(ui->page_6);

    QGridLayout *layout1 = new QGridLayout();
    layout1->addWidget(man_student);
    ui->page->setLayout(layout1);
    ui->stackedWidget->setLayout(layout1);

    QGridLayout *layout2 = new QGridLayout();
    layout2->addWidget(man_course);
    ui->page_2->setLayout(layout2);

    QGridLayout *layout3 = new QGridLayout();
    layout3->addWidget(ti_score);
    ui->page_3->setLayout(layout3);


    QGridLayout *layout4 = new QGridLayout();
    layout4->addWidget(t_score);
    ui->page_4->setLayout(layout4);

    QGridLayout *layout5 = new QGridLayout();
    layout5->addWidget(anal_gscore);
    ui->page_5->setLayout(layout5);

    QGridLayout *layout6 = new QGridLayout();
    layout6->addWidget(anal_score);
    ui->page_6->setLayout(layout6);


}

MainWindow::~MainWindow()
{
    delete ui;
}



///*将lineChart类对象和infoTable类对象放入tabWidget类对象
// 的不同的窗口中*/
//lineChart *linechart = new lineChart(this);
//infoTable *infotable = new infoTable(this);
//i=0;
//QHBoxLayout *layout=new QHBoxLayout;
//layout->addWidget(linechart);
//ui->tab_linechart->setLayout(layout);

//QHBoxLayout *layout1 = new QHBoxLayout;
//layout1->addWidget(infotable);
//ui->tab_infotable->setLayout(layout1);

void MainWindow::on_pushButton_clicked()
{
    ui->page->setVisible(true);

    ui->page_2->setVisible(false);
    ui->page_3->setVisible(false);
    ui->page_4->setVisible(false);
    ui->page_5->setVisible(false);
    ui->page_6->setVisible(false);
           // ui->page4->setVisible(true);
}

void MainWindow::on_pushButton_2_clicked()
{
    ui->page->setVisible(false);

    ui->page_2->setVisible(true);
    ui->page_3->setVisible(false);
    ui->page_4->setVisible(false);
    ui->page_5->setVisible(false);
    ui->page_6->setVisible(false);
}

void MainWindow::on_pushButton_3_clicked()
{
    ui->page->setVisible(false);

    ui->page_2->setVisible(false);
    ui->page_3->setVisible(true);
    ui->page_4->setVisible(false);
    ui->page_5->setVisible(false);
    ui->page_6->setVisible(false);
}

void MainWindow::on_pushButton_5_clicked()
{
    ui->page->setVisible(false);

    ui->page_2->setVisible(false);
    ui->page_3->setVisible(false);
    ui->page_4->setVisible(true);
    ui->page_5->setVisible(false);
    ui->page_6->setVisible(false);
}

void MainWindow::on_pushButton_6_clicked()
{
    ui->page->setVisible(false);

    ui->page_2->setVisible(false);
    ui->page_3->setVisible(false);
    ui->page_4->setVisible(false);
    ui->page_5->setVisible(true);
    ui->page_6->setVisible(false);
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->page->setVisible(false);

    ui->page_2->setVisible(false);
    ui->page_3->setVisible(false);
    ui->page_4->setVisible(false);
    ui->page_5->setVisible(false);
    ui->page_6->setVisible(true);
}
