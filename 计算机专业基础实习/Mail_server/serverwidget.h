#ifndef SERVERWIDGET_H
#define SERVERWIDGET_H

#include <QDebug>
#include <QDir>
#include <QMessageBox>
#include <QNetworkProxyFactory>
#include <QSqlDatabase>
#include <QSqlError>
#include <QSqlQuery>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTimer>
#include <QWidget>
#include <QVariantList>
#include <QtNetwork/QTcpSocket>

#define MY_IP "127.0.0.1"

QT_BEGIN_NAMESPACE
namespace Ui {
class ServerWidget;
}
QT_END_NAMESPACE

static QString usr_name;//当前登录用户id

class ServerWidget : public QWidget
{
    Q_OBJECT

public:

    explicit ServerWidget(QWidget *parent = nullptr);

    ~ServerWidget();

protected slots:

    void newClient();

    void readData();

    void send_head();

    void send_file();

private:

    Ui::ServerWidget *ui;

    QTcpServer *tcpServer;//监听套接字
    QTcpSocket *tcpSocket{};//通信套接字

    QTcpServer tcpServer_attach;
    QTcpSocket *tcpSocket_attach;

    QString path;
    QDir d;

    QFile file; //文件对象
    QString fileName; //文件名字
    qint64 fileSize{}; //文件大小
    qint64 recSize{}; //已经接受文件的大小

    QString Spath;
    QFile Sfile; //文件对象
    QString SfileName; //文件名字
    qint64 SfileSize{}; //文件大小
    qint64 sendSize{}; //已经接受文件的大小

    QTimer timer;
    void openDatabase();
    void login(QString message);
    void regist(QString message);
    void viewQuestion(QString message);
    void change(QString message);
    void sendtext(QString message);
    void download(QString message);
    void saveDraft(QString message);
    void unread();
    void sendData();

};

#endif // SERVERWIDGET_H
