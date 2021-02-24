#include "already.h"
#include "ui_already.h"
#include "all.h"

already::already(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::already)
{
    ui->setupUi(this);

    setFixedSize(800, 600);

    //连接服务器
    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "alreadyWindow connect success"<<endl;}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);

    //向服务器发送收件箱邮件查询请求
    QString message = "alreadyMail#uselessInf";
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
                        //qDebug()<<MailNum;

                        //生成控件
                        for(int j = 0;j < MailNum;j++) {

                                QString reciever = result.section("#",j*3+2,j*3+2);//获取邮件收件人
                                QString theme = result.section("#",j*3+3,j*3+3);//获取邮件主题
                                QString mainText = result.section("#",j*3+4,j*3+4);//获取邮件正文
                                QString all = reciever + "       " +  theme   ;

                                QLabel *l = new QLabel();
                                l->setObjectName(tr("1#"));
                                l->setText(all);
                                ui->layout->addWidget(l);

                                //添加打开按钮
                                QPushButton *p = new QPushButton();
                                p->setObjectName(tr("2#"));
                                p->setText("打开"+ theme);
                                ui->layoutOpen->addWidget(p);

                                //点击打开创建read窗口
                                connect(p, static_cast<void (QPushButton::*)(bool)>(&QPushButton::clicked),  [=](){

                                    read *r = new read(this,reciever,theme,mainText);
                                    QString title = "您向"+reciever + "发送的主题为"+theme+"的邮件";
                                    r->setWindowTitle({title});//主题作为窗口，顺便传参，在读窗口查找数据库中邮件内容
                                    r->show();
                                    //有附件
//                                    readWithFile *rwf = new readWithFile(this);
//                                    rwf->setWindowTitle({"0"});//主题作为窗口，顺便传参，在读窗口查找数据库中邮件内容
//                                    rwf->show();
                                }
                                );

                        }
                        }

                );
}

already::~already()
{
    delete ui;
}


void already::on_pushButton_3_clicked()
{

    //删除layout和layoutopen中的控件
    QLayoutItem *child;
     while ((child = ui->layout->takeAt(0)) != 0)
     {
            //setParent为NULL，防止删除之后界面不消失
            if(child->widget())
            {
                child->widget()->setParent(NULL);
            }

            delete child;
     }

     while ((child = ui->layoutOpen->takeAt(0)) != 0)
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
     QString request = "Classification_of_query2"       ;
     QString message = request + '#' +category + '#' +keyword;

     tcpSocket->write(message.toUtf8().data());

}



void already::on_inbox_clicked()
{
    this->close();
    Recieve *mw = new Recieve(this);
    mw->show();
}



void already::on_outbox_clicked()
{
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::warning(NULL, "warning", "您已在发件箱(●'◡'●)！", QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void already::on_draft_clicked()
{
    this->close();
    draft *mw = new draft(this);
    mw->show();
}

void already::on_trash_clicked()
{
    this->close();
    rubbish *mw = new rubbish(this);
    mw->show();
}
