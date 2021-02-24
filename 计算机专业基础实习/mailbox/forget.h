#ifndef FORGET_H
#define FORGET_H

#include "all.h"
#include "mainwindow.h"

namespace Ui {
class forget;
}

class forget : public QDialog
{
    Q_OBJECT

public:

    explicit forget(QWidget *parent = nullptr);

    ~forget();

    QString request;
    QString usr_name;
    QString usr_password;
    QString question;
    QString answer;
    QString return_sign;
    QTcpSocket *tcpSocket;

private slots:

    void on_confirmReg_clicked();

    void on_check_clicked();

private:

    Ui::forget *ui;

};

#endif // FORGET_H
