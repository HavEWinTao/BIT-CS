#include "forget.h"
#include "ui_forget.h"
#include "mainwindow.h"

#include <QMainWindow>

//忘记密码
forget::forget(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::forget)
{
    ui->setupUi(this);

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {
                qDebug() << "forget connect success";
            }
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);

    //接收服务端返回检索结果
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;

                    QString return_request = result.section("#",0,0);

                    if(return_request == "question"){
                        QString show_question = result.section("#",1,1);
                        ui->question->setText(show_question);//显示问题
                    }else if(return_request == "result"){
                        QString return_reject = result.section("#",1,1);
                        if(return_reject == "success"){
                            this->close();
                            MainWindow *mw = new MainWindow(this);
                            mw->show();
                        }else{
                            QMessageBox::warning(NULL, "warning", "回答错误", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                        }

                    }else{
                        QMessageBox::warning(NULL, "warning", "程序错误", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                    }

                    }
                );
}

forget::~forget()
{
    delete ui;
}

void forget::on_confirmReg_clicked()
{


    QString str1 = ui->password1->text();
    QString str2 = ui->password2->text();

    //密码判断
    int flagPswd = 0;
    if(str1 == NULL){
        ui->showResult_pswd->setText("请填写密码");
    }
    else if(str1.length() >= 6 && str1.length() <= 16){
        flagPswd = 1;
        ui->showResult_pswd->setText("√");
    }
    else
        ui->showResult_pswd->setText("请填写正确的密码");

    //判断确认密码
    int flagConfirm = 0;
    if(str1 == str2 && str1 != NULL){
        ui->showResult->setText("√");
        flagConfirm = 1;
    }
    else{
       ui->showResult->setText("请填写相同的密码");
    }

    //向服务器发送保密问题答案以及新密码
    request = "change";
    usr_name = ui->ID->text();
    answer = ui->answer->text();
    usr_password = ui->password1->text();
    QString message = request + '#' + usr_name + '#' + answer + '#' + usr_password;

    tcpSocket->write(message.toUtf8().data());


}

void forget::on_check_clicked()
{

    //向服务器发送用户名检索保密问题
    request = "viewQuestion";
    usr_name = ui->ID->text();
    QString message = request + '#' + usr_name;

    tcpSocket->write(message.toUtf8().data());
}
