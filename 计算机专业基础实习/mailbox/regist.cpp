#include "regist.h"
#include "ui_regist.h"
#include "login.h"

//登录
regist::regist(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::regist)
{
    ui->setupUi(this);

    setWindowTitle("注册");
    setFixedSize(705, 443);
    setStyleSheet("background-color:pink;");

    ui->password1->setEchoMode(QLineEdit::Password);
    ui->password2->setEchoMode(QLineEdit::Password);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "regist connect success"<<endl;}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);



}

void regist::on_reg_clicked()
{
    emit mySignal();
    this->close();
}
void regist::closeEvent(QCloseEvent *event)
{
    emit mySignal(); //发送信号
}

void regist::on_backF_clicked()
{
    this->close();
    login *log = new login(this);
    log->show();
}
regist::~regist()
{
    delete ui;
}

void regist::on_confirmReg_clicked()
{
    //str1与str2为密码
    QString str1 = ui->password1->text();
    QString str2 = ui->password2->text();
    //str3为用户名
    QString str3 = ui->ID->text();
    //4、5为密保问题
    QString str4 = ui->question->text();
    QString str5 = ui->answer->text();



    //用户名判断
    int flagID = 0;
    if (str3 == NULL)
    {
        ui->showResult_ID->setText("请填写用户名");
    }
    else if(str3.length() >= 6 && str3.length() <= 16){
        if((str3[0] >= 'a' && str3[0] <= 'z') || (str3[0] >= 'A' && str3[0] <= 'Z')){
            for(int i = 0; i < str3.length(); i++){
                if((str3[i] >= 'a' && str3[i] <= 'z') || (str3[i] >= 'A' && str3[i] <= 'Z') ||
                        (str3[i] >= '1' &&str3[i] <= '9') || str3[i] == '_')
                    continue;
                else
                    goto k;
            }
            ui->showResult_ID->setText("√");
            flagID = 1;
        }
        else
            goto k;
    }
    else{
       k:
       ui->showResult_ID->setText("请填写正确的用户名");
    }

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
    //总体判断
    if(flagID == 1 && flagPswd == 1 && flagConfirm == 1){

        QString request = "regist";
        QString message = request + '#' + str3 + '#' + str1 + '#' + str4 + '#' + str5;//合并登录信息

        //向服务器发送登录信息
        tcpSocket->write(message.toUtf8().data());

        //处理服务器返回信号
        connect(tcpSocket,&QTcpSocket::readyRead,
                    [=](){
                        QByteArray return_sign = tcpSocket->readAll();
                        QString result = return_sign;
                        qDebug() <<"result:"<<result<<endl;
                        if(result == "success"){
                            this->close();
                            MainWindow *mw = new MainWindow(this);
                            mw->show();
                        }else{
                            QApplication::setQuitOnLastWindowClosed(false);
                            QMessageBox::warning(NULL, "warning", "用户名重复，注册失败！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
                        }
                    });

//        this->close();
//        MainWindow *mw = new MainWindow(this);
//        mw->show();

    }
}
