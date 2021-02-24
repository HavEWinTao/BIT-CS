#include "write.h"
#include "ui_write.h"

#include "all.h"
//写信

Write::Write(QWidget *parent,QString a,QString b,QString c) :
    QMainWindow(parent),
    ui(new Ui::Write)
{
    ui->setupUi(this);

    setWindowTitle("写信");
    setFixedSize(896, 600);
    setStyleSheet("background-color:pink;");

    connect(ui->color,SIGNAL(clicked()),this,SLOT(slot_OpenColorPad()));
    connect(this,SIGNAL(signal_changeBackColor()),this,SLOT(slot_setBackColor()));

    if (a!=""){
        receiver =a;
        theme = b;
        maintext = c;
        ui->receiver->setText(a);
        ui->theme->setText(b);
        ui->maintext->setText(c);
    }
    //数据库发送邮件

    tcpSocket = new QTcpSocket(this);
    connect(tcpSocket, &QTcpSocket::connected,
            [=]()
            {qDebug() << "writeWidge connect success";}
            );
    QString ip = MY_IP;
    qint16 port = 8888;
    tcpSocket->connectToHost(QHostAddress(ip),port);
}

Write::~Write()
{
    if(NULL !=m_pColorDialog){delete m_pColorDialog;m_pColorDialog=NULL;}
    tcpSocket->disconnectFromHost();
    tcpSocket->close();
    delete ui;
}

void Write::on_reg_clicked()
{
    emit mySignal();
    this->close();
}

void Write::closeEvent(QCloseEvent *event)
{
    emit mySignal(); //发送信号
}

void Write::on_fontComboBox_currentFontChanged(const QFont &f)
{
    ui->maintext->setFont(f);
    fon = f;
}

void Write::on_underscore_clicked()
{
    if(flag1){
        fon.setUnderline(true);
        flag1 = 0;
    }
    else{
       fon.setUnderline(false);
       flag1 = 1;
    }
    ui->maintext->setFont(fon);
}

void Write::on_bold_clicked()
{
    if(flag2 == 1){
        flag2 = 0;
        fon.setWeight(75);
    }
    else{
        flag2 = 1;
        fon.setWeight(50);
    }
    ui->maintext->setFont(fon);
}

void Write::on_italic_clicked()
{
    fon.setItalic(flag3);
    if(flag3 == 1){
        flag3 = 0;
    }
    else{
        flag3 = 1;
    }
    ui->maintext->setFont(fon);
}

void Write::on_fontSize_textChanged(const QString &arg1)
{
    int size = arg1.toInt();
    fon.setPointSize(size);
    ui->maintext->setFont(fon);
}

void Write::on_color_clicked()
{
    m_pColorDialog = new QColorDialog;
    connect(m_pColorDialog,SIGNAL(colorSelected(QColor)),this,SLOT(slot_getColor(QColor)));
    m_myPalette = ui->maintext->palette();
    m_pColorDialog->exec();
}

void Write::slot_getColor(QColor cor)
{
    m_red=cor.red();
    m_green=cor.green();
    m_blue=cor.blue();
    emit signal_changeBackColor();
}

void Write::slot_setBackColor()
{
    QString colorStr=QString("color:rgb(%1,%2,%3);").arg(m_red).arg(m_green).arg(m_blue);
    ui->maintext->setStyleSheet(colorStr);
}


void Write::on_pushButton_clicked()//发送按钮
{
    receiver =ui->receiver->text();
    theme = ui->theme->text();
    maintext = ui->maintext->toPlainText();
    if(maintext == NULL || theme == NULL || receiver == NULL)
    {
        QApplication::setQuitOnLastWindowClosed(false);
        QMessageBox msgBox;   // 生成对象
        msgBox.setText("请填写必填项");    // 设置文本
        msgBox.exec();  // 执行
        //QMessageBox::information(NULL,"提示","请填写必填项",QMessageBox::Yes|QMessageBox::Yes,QMessageBox::No);
        return;
    }
    //记录发送信息
    QString request = "sendtext";
    QString Cc_people = "DEFAULT";
    QString CS_people = "DEFAULT";
    QString recipient = ui->receiver->text();
    Cc_people = ui->receiver_3->text();
    CS_people = ui->receiver_4->text();
    QString MailTheme = ui->theme->text();
    QString MainText = ui->maintext->toPlainText();
    QDateTime current_date_time = QDateTime::currentDateTime();
    QString now_time = current_date_time.toString("yyyy-MM-dd hh:mm::ss.zzz");
    //向服务器发送邮件
    QString flag_attach;
    QString the_name = "-";
    if (filepath=="") {
        flag_attach = "false";
    } else {
        flag_attach = "true";
        QFileInfo temp(filepath);
        the_name = temp.fileName();
    }
    QString message = request + '#' + recipient + '#' + now_time + '#' + MailTheme + '#' + MainText + "#"  +flag_attach+"#"+ Cc_people + '#' + CS_people + '#' +the_name;//合并登录信息
    tcpSocket->write(message.toUtf8().data());

    if (filepath!=""){
        QString ip = MY_IP;
        tcpSocket_attach = new QTcpSocket(this);
        tcpSocket_attach->connectToHost(QHostAddress(ip),8080);
        fileSize = 0;
        sendSize = 0;
        timer.start(200);
        connect(tcpSocket_attach,&QTcpSocket::connected,this,&Write::send_head);
        connect(tcpSocket_attach,&QTcpSocket::bytesWritten,this,&Write::send_file);
    }

    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::information(NULL, "邮件发送", "邮件发送成功！",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);

}

void Write::on_pushButton_3_clicked()//关闭按钮
{
    this->close();
}

void Write::on_pushButton_2_clicked()//存草稿按钮
{
    //记录草稿信息
    QString request = "saveDraft";
    QString recipient = ui->receiver->text();
    QString Cc_people = "DEFAULT";
    QString CS_people = "DEFAULT";
    Cc_people = ui->receiver_3->text();
    CS_people = ui->receiver_4->text();
    QString MailTheme = ui->theme->text();
    QString MainText = ui->maintext->toPlainText();
    QString message = request + '#' + recipient + '#' + MailTheme + '#' + MainText + "#" +"#"+ Cc_people + '#' + CS_people;//合并登录信息

    //向服务器发送邮件
    tcpSocket->write(message.toUtf8().data());
    QApplication::setQuitOnLastWindowClosed(false);
    QMessageBox::information(NULL, "邮件发送", "草稿储存成功！",
                             QMessageBox::Yes | QMessageBox::No, QMessageBox::Yes);
}

void Write::on_attach_clicked()
{
    filepath = QFileDialog::getOpenFileName(this);
    ui->attachName->setText(filepath);
}

void Write::send_head(){
    QFileInfo info(filepath);
    fileName = info.fileName();
    fileSize = info.size();
    file.setFileName(filepath);
    file.open(QIODevice::ReadOnly);
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream << fileSize << fileName;
    tcpSocket_attach->write(array);
}

void Write::send_file(){
    connect(&timer, &QTimer::timeout,
            [=]()
            {
                //关闭定时器
                timer.stop();
                send_data();
            });
}

void Write::send_data(){
    //发送文件
    if(sendSize < fileSize){
        QByteArray array = file.read(1024*10);//读取一段内容
        tcpSocket_attach->write(array);//发送一段内容
        sendSize += array.size();
        timer.start(200);
    }
    if(sendSize == fileSize){
        file.close();//关闭文件
    }
}
