#include "readwithfile.h"
#include "ui_readwithfile.h"

readWithFile::readWithFile(QWidget *parent,QString a,QString b,QString c,QString d) :
    QDialog(parent),
    rsender(a),
    rMailTheme(b),
    rMainText(c),
    rAttach(d),
    ui(new Ui::readWithFile)
{

    ui->setupUi(this);

    setFixedSize(566, 508);

    ui->label_3->setText(rsender);
    ui->label_4->setText(rMailTheme);
    ui->textBrowser->insertPlainText(rMainText);
    ui->label_7->setText(rAttach);

    path = dir.currentPath() + "/attach/";
    if (!dir.exists(path)){
        bool res = dir.mkpath(path);
        qDebug() << "attach created:" << res;
    }
    //监听套接字
    tcpServer_attach.listen(QHostAddress::Any,8889);
    connect(&tcpServer_attach,&QTcpServer::newConnection,this,&readWithFile::newClient);
    tcpSocket = new QTcpSocket(this);
}

readWithFile::~readWithFile()
{
    delete ui;
}

void readWithFile::newClient(){
    //创建于客户端的套接字
    tcpSocket_attach = tcpServer_attach.nextPendingConnection();

    //关联读取数据信号
    connect(tcpSocket_attach,&QTcpSocket::readyRead,this,&readWithFile::readData);
    fileSize = 0;
    recSize = 0;
}

void readWithFile::on_download_clicked()
{
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);
    QString request = "download";
    QString message = request + '#' + rAttach;
    tcpSocket->write(message.toUtf8().data());

}

void readWithFile::readData(){
    if (fileSize==0){
        QByteArray array = tcpSocket_attach->readAll();
        QDataStream stream(&array,QIODevice::ReadOnly);//把套接字与数据流绑定
        stream >> fileSize >> fileName;//获取文件大小、文件名
        qDebug() << "filesize:" << fileSize;
        file.setFileName(path+rAttach);
        file.open(QIODevice::WriteOnly);
    }
    if (recSize < fileSize){
        QByteArray array = tcpSocket_attach->readAll();
        file.write(array);
        recSize += array.size();
    }
    if (recSize==fileSize){
        qDebug() << "recSize:" << recSize;
        file.close();
        tcpSocket_attach->disconnectFromHost();
    }
}

void readWithFile::on_reply_clicked()
{
    Write *mw = new Write(this,rsender,rMailTheme,rMainText);
    mw->receiver.insert(0,rsender);
    mw->show();
}
