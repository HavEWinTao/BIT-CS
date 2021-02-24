#include "recieve.h"
#include "ui_recieve.h"
#include <QLabel>
#include "already.h"
#include "all.h"

Recieve::Recieve(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Recieve)
{
    ui->setupUi(this);

    setWindowTitle("收件箱");
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

    //向服务器发送收件箱邮件查询请求
    QString message = "ReceiveMail#uselessInf";
    tcpSocket->write(message.toUtf8().data());

    //处理服务器返回信号
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;
                    //qDebug() <<result;
                    QString return_request = result.section("#",0,0);
                    //qDebug()<<return_request;//测试函数

                        int MailNum = result.section("#",1,1).toInt();//收取邮件总数
                        qDebug()<<MailNum;

                        //生成控件
                        for(int j = 0;j < MailNum;j++) {

                                QString sender = result.section("#",j*4+2,j*4+2);//获取邮件发件人
                                QString theme = result.section("#",j*4+3,j*4+3);//获取邮件主题
                                QString mainText = result.section("#",j*4+4,j*4+4);//获取邮件正文
                                QString FileName = result.section("#",j*4+5,j*4+5);
                                QString all = sender + "       " +  theme   ;

                                QLabel *l = new QLabel();
                                l->setObjectName(tr("1#"));
                                l->setText(all);
                                ui->layout1->addWidget(l);

                                //添加打开按钮
                                QPushButton *p = new QPushButton();
                                p->setObjectName(tr("2#"));
                                p->setText("打开"+ theme);
                                ui->layoutOpen1->addWidget(p);

                                //添加删除按钮
                                QPushButton *q = new QPushButton();
                                q->setObjectName(tr("3#"));
                                q->setText("删除" + theme);
                                ui->layoutOpen1->addWidget(q);
                                //点击打开创建read窗口
                                connect(p, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){
                                    QString title = sender +"向您"+ "发送的主题为"+theme+"的邮件";
                                    qDebug() << FileName;
                                    if (FileName == "-"){
                                        read *r = new read(this,sender,theme,mainText);
                                        r->setWindowTitle({title});//主题作为窗口，顺便传参，在读窗口查找数据库中邮件内容
                                        r->show();
                                    } else {
                                    //有附件
                                        readWithFile *rwf = new readWithFile(this,sender,theme,mainText,FileName);
                                        rwf->setWindowTitle({title});//主题作为窗口，顺便传参，在读窗口查找数据库中邮件内容
                                        rwf->show();
                                    }
                                }
                                );

                                //点击删除，将对应邮件移进垃圾箱
                                connect(q, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){

                                    //删除layout和layoutopen中的控件
                                    QLayoutItem *child;
                                     while ((child = ui->layout1->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     while ((child = ui->layoutOpen1->takeAt(0)) != 0)
                                     {
                                            //setParent为NULL，防止删除之后界面不消失
                                            if(child->widget())
                                            {
                                                child->widget()->setParent(NULL);
                                            }

                                            delete child;
                                     }

                                     //发送删除信息

                                     QString category = ui->comboBox->currentText();//排序关键字
                                     QString keyword = ui->lineEdit->text();//检索关键字

                                     QString request = "delete"       ;
                                     QString message = request + '#' +category + '#' +keyword+ '#' +sender+ '#' +theme;

                                     tcpSocket->write(message.toUtf8().data());

                                }
                                );
                        }
                        }

                );

}

Recieve::~Recieve()
{
    delete ui;
}


void Recieve::on_reg_clicked()
{
    emit mySignal();
    this->close();
}
void Recieve::closeEvent(QCloseEvent *event)
{
    emit mySignal(); //发送信号
}

void Recieve::on_pushButton_3_clicked()
{
    //删除layout和layoutopen中的控件
    QLayoutItem *child;
     while ((child = ui->layout1->takeAt(0)) != 0)
     {
            //setParent为NULL，防止删除之后界面不消失
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }

            delete child;
     }

     while ((child = ui->layoutOpen1->takeAt(0)) != 0)
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
     QString request = "Classification_of_query"       ;
     QString message = request + '#' +category + '#' +keyword;

     tcpSocket->write(message.toUtf8().data());

}

void Recieve::on_outbox_clicked()
{
    this->close();
    already *mw = new already(this);
    mw->show();
}

void Recieve::on_draft_clicked()
{
    this->close();
    draft *mw = new draft(this);
    mw->show();
}

void Recieve::on_trash_clicked()
{
    this->close();
    rubbish *mw = new rubbish(this);
    mw->show();
}

void Recieve::on_inbox_clicked()
{
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL, "warning", "您已在收件箱(●'◡'●)！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}
