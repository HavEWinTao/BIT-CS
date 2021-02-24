#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTimer>
#include <QDateTime>

//主窗口
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{

    ui->setupUi(this);

    setWindowTitle("邮箱首页");
    setFixedSize(823, 601);
    QTimer *timer = new QTimer(this);
    connect(timer,SIGNAL(timeout()),this,SLOT(timerUpdate()));
    timer->start(1000);

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "MainWindow connect success"<<endl;}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);


    //向服务器发送查询请求
    QString message = "unread#useless";
    tcpSocket->write(message.toUtf8().data());

    //处理服务器返回信号
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;
                    ui->NotRead->setText("有"+result+"未读邮件");
                    ui->dtime-> setScaledContents(true);
                    }
                );

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_reg_clicked()
{
    emit mySignal();
    this->close();
}
void MainWindow::timerUpdate(void)
{
    QDateTime time = QDateTime::currentDateTime();
    QString str = time.toString("yyyy-MM-dd hh:mm:ss dddd");
    ui->dtime->setText("现在是 " + str);
}

void MainWindow::on_goWrite_clicked()
{
    Write *mw = new Write(this);
    mw->show();
}


void MainWindow::on_goRecieve_clicked()
{
    Recieve *mw = new Recieve(this);
    mw->show();
}

void MainWindow::on_goRecieve2_clicked()
{
    Recieve *mw = new Recieve(this);
    mw->show();
}

void MainWindow::my_show_slot()
{
    this->show();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    emit mySignal(); //发送信号
}

void MainWindow::on_goAlready_clicked()
{
    already *mw = new already(this);
    mw->show();
}

void MainWindow::on_goDraft_clicked()
{
    draft *mw = new draft(this);
    mw->show();
}

void MainWindow::on_goRubbish_clicked()
{
    rubbish *mw = new rubbish(this);
    mw->show();
}

void MainWindow::on_NotRead_linkActivated(const QString &link)
{

}
