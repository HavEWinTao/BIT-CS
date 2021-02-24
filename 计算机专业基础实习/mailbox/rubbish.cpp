#include "rubbish.h"
#include "ui_rubbish.h"
#include "all.h"

rubbish::rubbish(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::rubbish)
{
    ui->setupUi(this);

    setFixedSize(800, 600);

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "receiveWindow connect success"<<endl;}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);

    //向服务器发送垃圾箱箱邮件查询请求
    QString message = "rubbishMail#uselessInf";
    tcpSocket->write(message.toUtf8().data());

    //处理服务器返回信号
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;
                    //qDebug() <<result;
                    QString return_request = result.section("#",0,0);
                    //qDebug()<<return_request;//测试函数

                    //恢复成功显示窗口
                    if(return_request == "recoverSuccess"){

                        QApplication::setQuitOnLastWindowClosed(false);
                        QMessageBox::warning(NULL, "warning", "邮件恢复成功", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                    }

                    //删除成功显示窗口
                    if(return_request == "deleteSuccess"){

                        QApplication::setQuitOnLastWindowClosed(false);
                        QMessageBox::warning(NULL, "warning", "邮件彻底删除成功", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                    }

                        int MailNum = result.section("#",1,1).toInt();//收取邮件总数
                        qDebug()<<MailNum;

                        //生成控件
                        for(int j = 0;j < MailNum;j++) {

                                QString sender = result.section("#",j*3+2,j*3+2);//获取邮件发件人
                                QString theme = result.section("#",j*3+3,j*3+3);//获取邮件主题
                                QString mainText = result.section("#",j*3+4,j*3+4);//获取邮件正文
                                QString all = sender + "       " +  theme   ;

                                QLabel *l = new QLabel();
                                l->setObjectName(tr("1#"));
                                l->setText(all);
                                ui->layout_4->addWidget(l);

                                //添加恢复按钮
                                QPushButton *p = new QPushButton();
                                p->setObjectName(tr("2#"));
                                p->setText("恢复"+ theme);
                                ui->layoutOpen_4->addWidget(p);

                                //添加删除按钮
                                QPushButton *q = new QPushButton();
                                q->setObjectName(tr("3#"));
                                q->setText("删除" + theme);
                                ui->layoutOpen_4->addWidget(q);

                                //点击恢复向服务器发送恢复信号
                                connect(p, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){

                                    //删除layout和layoutopen中的控件
                                    QLayoutItem *child;
                                     while ((child = ui->layout_4->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     while ((child = ui->layoutOpen_4->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     //发送恢复信息
                                     QString request = "recover"       ;
                                     QString message = request + '#' +sender + '#' +theme;

                                     tcpSocket->write(message.toUtf8().data());

                                }
                                );

                                //点击删除，将对应邮件彻底删除
                                connect(q, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){

                                    //删除layout和layoutopen中的控件
                                    QLayoutItem *child;
                                     while ((child = ui->layout_4->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     while ((child = ui->layoutOpen_4->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     //发送恢复信息
                                     QString request = "rubbishDelete"       ;
                                     QString message = request + '#' +sender + '#' +theme;

                                     tcpSocket->write(message.toUtf8().data());

                                }
                                );
                        }
                        }

                );
}

rubbish::~rubbish()
{
    delete ui;
}


void rubbish::on_pushButton_3_clicked()
{
    //删除layout和layoutopen中的控件
    QLayoutItem *child;
     while ((child = ui->layout_4->takeAt(0)) != 0)
     {
            //setParent为NULL，防止删除之后界面不消失
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }

            delete child;
     }

     while ((child = ui->layoutOpen_4->takeAt(0)) != 0)
     {
            //setParent为NULL，防止删除之后界面不消失
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }

            delete child;
     }

     //发送查询信息

     QString category = ui->comboBox->currentText();//排序关键字
     QString keyword = ui->lineEdit->text();//检索关键字
     QString request = "Classification_of_query_rubbish"       ;
     QString message = request + '#' +category + '#' +keyword;

     tcpSocket->write(message.toUtf8().data());
}

void rubbish::on_pushButton_12_clicked()
{
    this->close();
    Recieve *mw = new Recieve(this);
    mw->show();
}

void rubbish::on_outbox_clicked()
{
    this->close();
    already *mw = new already(this);
    mw->show();
}

void rubbish::on_draft_clicked()
{
    this->close();
    draft *mw = new draft(this);
    mw->show();
}

void rubbish::on_trash_clicked()
{
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL, "warning", "您已在垃圾箱(●'◡'●)！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}
