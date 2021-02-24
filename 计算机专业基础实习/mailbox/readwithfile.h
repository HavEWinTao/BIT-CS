#ifndef READWITHFILE_H
#define READWITHFILE_H

#include <all.h>

namespace Ui {
class readWithFile;
}

class readWithFile : public QDialog
{
    Q_OBJECT

public:

    explicit readWithFile(QWidget *parent = nullptr,QString a = "default",QString b = "default",QString c = "default",QString d="default");

    ~readWithFile();

    QString rsender;//邮件发件人
    QString rMailTheme;//邮件主题
    QString rMainText;//邮件正文
    QString rAttach;

private slots:

    void on_download_clicked();

    void newClient(); //新的客户端

    void readData(); //读取文件数据

    void on_reply_clicked();

private:

    Ui::readWithFile *ui;

    QTcpServer tcpServer_attach;
    QTcpSocket *tcpSocket_attach;
    QTcpSocket *tcpSocket;//通信套接字


    QDir dir;
    QString path;

    QFile file; //文件对象
    QString fileName; //文件名字
    qint64 fileSize; //文件大小
    qint64 recSize; //已经接受文件的大小

};

#endif // READWITHFILE_H
