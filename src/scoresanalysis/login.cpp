#include "login.h"
#include "ui_login.h"
#include "QMessageBox"
#include <QtSql/QSqlRelationalDelegate>
#include <QSqlRecord>

Login::Login(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::Login)
{
    ui->setupUi(this);
}

Login::~Login()
{
    delete ui;
}

void Login::on_loginButton_clicked()
{
    QString userName = ui->nameedit->text();
    QString userPassword = ui->passwordedit->text();
    userModel = new QSqlQueryModel;
    userModel->setQuery(QString("SELECT * FROM admin"));
    int i = 0;
    for (i = 0; i < userModel->rowCount(); ++i) {
        QSqlRecord record = userModel->record(i);
        if(record.value(0).toString() == userName)
        {
            if(record.value(1).toString() == userPassword)
            {
                emit enter();
                break;
            } else
            {
                 QMessageBox::warning(this,tr("登陆"),tr("密码错误"),tr("确定"));
                 ui->passwordedit->clear();
                 ui->passwordedit->setFocus();
                 break;
            }
        }
    }
    if(i==userModel->rowCount())
    {
         QMessageBox::warning(this,tr("登陆"),tr("此用户不存在"),tr("确定"));
         ui->nameedit->clear();
         ui->passwordedit->clear();
         ui->nameedit->setFocus();
    }


}

void Login::on_nameedit_textChanged(QString )
{
    ui->loginButton->setEnabled(true);
}

void Login::on_helpButton_clicked()
{
    emit help();
}

void Login::on_passwordedit_returnPressed()
{
    ui->loginButton->click();
}

