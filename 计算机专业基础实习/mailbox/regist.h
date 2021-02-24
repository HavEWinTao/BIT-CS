#ifndef REGIST_H
#define REGIST_H

#include "mainwindow.h"
#include "forget.h"
#include "all.h"

namespace Ui {
class regist;
}

class regist : public QDialog
{
    Q_OBJECT

public:

    explicit regist(QWidget *parent = nullptr);

    ~regist();

    QString request;
    QString usr_name;
    QString usr_password;
    QString question;
    QString answer;
    QTcpSocket *tcpSocket;

signals:

    void mySignal();

private slots:

    void on_reg_clicked();

    void on_backF_clicked();

    void on_confirmReg_clicked();

private:

    Ui::regist *ui;

protected:

    virtual void closeEvent(QCloseEvent *event);

};

#endif // REGIST_H
