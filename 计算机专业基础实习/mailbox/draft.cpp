#include "draft.h"
#include "ui_draft.h"
#include "all.h"
#include "write.h"

draft::draft(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::draft)
{
    ui->setupUi(this);

    setFixedSize(800, 600);

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "draftWindow connect success"<<endl;}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);

    //向服务器发送草稿箱邮件查询请求
    QString message = "draftMail#uselessInf";
    tcpSocket->write(message.toUtf8().data());

    //处理服务器返回信号
    connect(tcpSocket,&QTcpSocket::readyRead,
                [=](){
                    QByteArray return_sign = tcpSocket->readAll();
                    QString result = return_sign;
                    //qDebug() <<result;
                    QString return_request = result.section("#",0,0);
                    //qDebug()<<return_request;//测试函数

                    //删除成功显示窗口
                    if(return_request == "deleteSuccess"){

                        QApplication::setQuitOnLastWindowClosed(false);
                        QMessageBox::warning(NULL, "warning", "邮件被移至垃圾箱", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

                    }

                        int MailNum = result.section("#",1,1).toInt();//收取邮件总数
                        qDebug()<<MailNum;

                        //生成控件
                        for(int j = 0;j < MailNum;j++) {

                                QString reciever = result.section("#",j*3+2,j*3+2);//获取邮件收件人
                                QString theme = result.section("#",j*3+3,j*3+3);//获取邮件主题
                                QString mainText = result.section("#",j*3+4,j*3+4);//获取邮件正文
                                QString all = reciever + "       " +  theme   ;

                                QLabel *l = new QLabel();
                                l->setObjectName(tr("1#"));
                                l->setText(all);
                                ui->layout_4->addWidget(l);

                                //添加编辑按钮
                                QPushButton *p = new QPushButton();
                                p->setObjectName(tr("2#"));
                                p->setText("编辑"+ theme);
                                ui->layoutOpen_4->addWidget(p);

                                //添加删除按钮
                                QPushButton *q = new QPushButton();
                                q->setObjectName(tr("3#"));
                                q->setText("删除" + theme);
                                ui->layoutOpen_4->addWidget(q);

                                //点击编辑重写邮件
                                connect(p, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){

                                     Write *mw = new Write(this,reciever,theme,mainText);
                                     mw->receiver.insert(0,reciever);
                                     mw->show();
                                }
                                );

                                //点击删除，将对应编辑邮件移动到垃圾箱
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

                                     //发送删除草稿信息
                                     QString request = "draftDelete"       ;
                                     QString message = request + '#' +reciever + '#' +theme;

                                     tcpSocket->write(message.toUtf8().data());

                                }
                                );
                        }
                        }

                );

}

draft::~draft()
{
    delete ui;
}



void draft::on_inbox_clicked()
{
    this->close();
    Recieve *mw = new Recieve(this);
    mw->show();
}

void draft::on_outbox_clicked()
{
    this->close();
    already *mw = new already(this);
    mw->show();
}

void draft::on_draft_2_clicked()
{
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL, "warning", "您已在草稿箱(●'◡'●)！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void draft::on_trash_clicked()
{
    this->close();
    rubbish *mw = new rubbish(this);
    mw->show();
}
