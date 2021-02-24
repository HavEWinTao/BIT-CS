#include "login.h"
#include "ui_login.h"

#include "all.h"

//登录
login::login(QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::login)
{
    ui->setupUi(this);
    setWindowTitle("登录");
    setFixedSize(350, 375);
    setStyleSheet("background-color:pink;");
    ui->password->setEchoMode(QLineEdit::Password);

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "login connect success";}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);
}

void login::on_reg_clicked()
{
    this->close();
    regist *reg = new regist(this);
    reg->show();
}

void login::on_confirm_clicked()//这里需要判断用户密码操作
{
    //记录登陆者信息
    QString request = "login";
    usr_name = ui->address->text();
    usr_password = ui->password->text();
    QString message = request + '#' + usr_name + '#' + usr_password;//合并登录信息
    //向服务器发送登录信息
    tcpSocket->write(message.toUtf8().data());
    //处理服务器返回信号
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;
                    //qDebug() <<result<<endl;
                    if(result == "success"){
                        this->close();
                        MainWindow *mw = new MainWindow(this);
                        mw->show();
                    }else{
                        QApplication::setQuitOnLastWindowClosed(false);
                        QMessageBox msgBox;   // 生成对象
                        msgBox.setText("请填写必填项");    // 设置文本
                        msgBox.exec();  // 执行
                    }
                });
}

void login::my_show_slot()
{
    this->show();
}

login::~login()
{
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    delete ui;
}

void login::on_forgetPswd_clicked()
{
    this->close();
    forget *reg = new forget(this);
    reg->show();
}
