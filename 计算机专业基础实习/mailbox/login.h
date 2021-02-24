#ifndef LOGIN_H
#define LOGIN_H


#include "regist.h"
#include "mainwindow.h"
#include "forget.h"
#include "all.h"

QT_BEGIN_NAMESPACE
namespace Ui {
class login;
}QT_END_NAMESPACE

class login : public QDialog
{
    Q_OBJECT

public:

    login(QWidget *parent = nullptr);

    ~login();

    QString usr_name;

    QString usr_password;

private slots:

    void on_reg_clicked();

    void on_confirm_clicked();

    void my_show_slot();

    void on_forgetPswd_clicked();

private:

    Ui::login *ui;

    QPushButton b1;

    QTcpSocket *tcpSocket;
    QString return_sign;

};

#endif // LOGIN_H
