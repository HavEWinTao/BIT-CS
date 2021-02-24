#include "serverwidget.h"
#include "ui_serverwidget.h"

ServerWidget::ServerWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ServerWidget)
{
    ui->setupUi(this);

    //创建附件储存路径
    path = d.currentPath() + "/attach/";
    if (!d.exists(path)){
        bool res = d.mkpath(path);
        if (res){
            ui->textEdit->append("attach created success");
        } else {
            ui->textEdit->append("attach created fail");
        }
    }

    openDatabase();


    /*建立通信*/
    //监听套接字
    tcpServer = new QTcpServer(this);
    tcpServer->listen(QHostAddress::Any,8888);//服务器端口号8888(邮件)


    tcpServer_attach.listen(QHostAddress::Any,8080);//服务器端口号8080(文件)
    //接收文件
    connect(&tcpServer_attach,&QTcpServer::newConnection,this,&ServerWidget::newClient);

    QString ip = MY_IP;
    tcpSocket_attach = new QTcpSocket(this);
    //发送界面
    connect(tcpSocket_attach,&QTcpSocket::connected,this,&ServerWidget::send_head);
    connect(tcpSocket_attach,&QTcpSocket::bytesWritten,this,&ServerWidget::send_file);

    //建立连接,判断要进行的操作
    connect(tcpServer,&QTcpServer::newConnection,
             [=](){
              //取出建立好链接的套接字
              tcpSocket = tcpServer->nextPendingConnection();
              //获取对方的IP和端口
              QString ip = tcpSocket->peerAddress().toString();
              qint16 port = tcpSocket->peerPort();
              QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);
              ui->textEdit->append(temp);
              //获取请求类别
              connect(tcpSocket,&QTcpSocket::readyRead,
                      [=](){
                          QByteArray receive = tcpSocket->readAll();//接受客户端发送的消息
                          QString message = receive;//转换消息格式
                          ui->textEdit->append(message);//消息内容
                          QString request = message.section("#",0,0);//提取请求类别
                          ui->textEdit->append(request);
                          //执行请求
                          if(request == "login"){//正常登录
                              login(message);
                          } else if(request == "regist"){//注册
                              regist(message);
                          } else if (request == "viewQuestion") {//查看保密问题
                              viewQuestion(message);
                          } else if (request == "change") {//验证保密问题
                              change(message);
                          } else if (request == "sendtext") {//发送邮件
                              sendtext(message);
                          } else if (request=="download") {
                              download(message);
                          } else if (request == "saveDraft") {//储存草稿
                              saveDraft(message);
                          } else if (request == "unread") {//查询未读邮件数
                              unread();
                          } else if (request == "ReceiveMail") {//窗口初始化查询用户收件箱
                              ui->textEdit->append(usr_name);
                              //作为收件人
                              QSqlQuery querya;
                              querya.prepare(QString("update tb_mails set r_read_statu = 1 where recipient=:usr_name and r_read_statu = 0"));
                              querya.bindValue(":usr_name",usr_name);
                              querya.exec();
                              if (querya.next()) {
                                  ui->textEdit->append(querya.value(0).toString());}
                              else {
                                  ui->textEdit->append(querya.lastError().text());
                              }
                              //作为抄送人
                              QSqlQuery queryb;
                              queryb.prepare(QString("update tb_mails set c_read_statu = 1 where Cc_people=:usr_name and c_read_statu = 0"));
                              queryb.bindValue(":usr_name",usr_name);
                              if (!queryb.exec()) {
                                  ui->textEdit->append(queryb.lastError().text());}
                              else {
                                  ui->textEdit->append("queryb success!");
                              }
                              //作为密送人
                              QSqlQuery queryc;
                              queryc.prepare(QString("update tb_mails set e_read_statu=1 where CS_people=:usr_name and e_read_statu=0"));
                              queryc.bindValue(":usr_name",usr_name);
                              if (!queryc.exec()) {
                                  ui->textEdit->append(queryc.lastError().text());}
                              else {
                                  ui->textEdit->append("queryc success!");
                              }
                              /*服务端在数据库邮件表中根据用户名usr_name查询所有接受邮件，返回结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=0 ) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=0) or (CS_people = :usr_name and e_preserve_statu=0)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' +num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText,file from tb_mails where (recipient=:usr_name and r_preserve_statu=0) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=0) or (CS_people = :usr_name and e_preserve_statu=0);"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                  ui->textEdit->append(query2.value(0).toString());
                                  return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString()+'#'+query2.value(3).toString();
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#2#发件人1#主题1#正文1#发件人2#主题2#正文2";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将所有未排序邮件检索结果发送到客户端
                          } else if (request == "Classification_of_query2") {//分类别关键字查询用户发件箱
                              QString category = message.section("#",1,1);//排序关键字
                              QString keyword = message.section("#",2,2);//检索关键字
                              ui->textEdit->append(category);
                              ui->textEdit->append("成功连接查询客户端！");
                              /*服务端在数据库邮件表中根据用户名usr_name、关键字查询所有接受邮件并按keyword要求排序，返回拼接字符串结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where sender=:usr_name and s_preserve_statu=0"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' +num_mails;
                              QSqlQuery query2;
                              if (category=="时间") {
                                  ui->textEdit->append("query 时间");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "sender=:usr_name and s_preserve_statu=0"
                                                         ") order by time;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              } else if (category=="发件人") {
                                  ui->textEdit->append("query 发件人");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "sender=:usr_name and s_preserve_statu=0"
                                                         ") order by sender;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              } else if (category=="主题") {
                                  ui->textEdit->append("query 主题");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "sender=:usr_name and s_preserve_statu=0"
                                                         ") order by title;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              }
                              ui->textEdit->append(return_string);
                              //QString return_string = "return_mail#1#发件人1#主题1#正文1";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将相关邮件排序检索结果发送到客户端
                          } else if(request == "delete") {//删除接受的邮件
                              QString category = message.section("#",1,1);//获取排序关键字
                              QString keyword = message.section("#",2,2);//获取检索关键字
                              QString sender = message.section("#",3,3);//获取邮件发件人
                              QString theme = message.section("#",4,4);//获取邮件主题
                              /*服务端先在数据库中根据发件人sender和主题theme将对应邮件移至垃圾箱，再同上用关键字查询用户收件箱，返回邮件排序检索结果*/
                              //移动到垃圾箱
                              //作为收件人
                              QSqlQuery querya;
                              querya.prepare(QString("update tb_mails set r_preserve_statu=1 where recipient=:usr_name and sender=:sender and title=:theme"));
                              querya.bindValue(":sender",sender);
                              querya.bindValue(":usr_name",usr_name);
                              querya.bindValue(":theme",theme);
                              if(querya.exec()) {
                                  ui->textEdit->append("querya success!");
                              }
                              else{
                                   ui->textEdit->append(querya.lastError().text());
                              }
                              //作为抄送人
                              QSqlQuery queryb;
                              queryb.prepare(QString("update tb_mails set c_preserve_statu=1 where Cc_people=:usr_name and sender=:sender and title=:theme"));
                              queryb.bindValue(":sender",sender);
                              queryb.bindValue(":usr_name",usr_name);
                              queryb.bindValue(":theme",theme);
                              if(querya.exec()) {
                                  ui->textEdit->append("queryb success!");
                              } else {
                                   ui->textEdit->append(queryb.lastError().text());
                              }
                              //作为密送人
                              QSqlQuery queryc;
                              queryc.prepare(QString("update tb_mails set e_preserve_statu=1 where CS_people=:usr_name and sender=:sender"
                                                    "and title=:theme"));
                              queryc.bindValue(":sender",sender);
                              queryc.bindValue(":usr_name",usr_name);
                              queryc.bindValue(":theme",theme);
                              if (querya.exec()) {
                                  ui->textEdit->append("queryc success!");
                              } else {
                                   ui->textEdit->append(queryc.lastError().text());
                              }
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=0) or (Cc_people = :usr_name and c_preserve_statu=0) or (CS_people = :usr_name and e_preserve_statu=0)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.value(0).toString());
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#'+num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where(recipient=:usr_name and r_preserve_statu=0) or (Cc_people = :usr_name and c_preserve_statu=0) or (CS_people = :usr_name and e_preserve_statu=0)"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.bindValue(":keyword",keyword);
                              query2.bindValue(":category",category);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#1#删除后的发件人1#删除后的主题1#删除后的正文1";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将邮件检索结果发送到客户端
                          } else if (request == "alreadyMail") {//窗口初始化查询用户发件箱
                              /*服务端在数据库邮件表中根据用户名usr_name查询所有发送邮件，返回结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where sender=:usr_name and s_preserve_statu = 0 and send_statu=1;"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#'+num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where sender=:usr_name and s_preserve_statu = 0 and send_statu=1"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString()+ '#' + query2.value(1).toString()+ '#' + query2.value(2).toString();
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#2#发件人1#主题1#正文1#发件人2#主题2#正文2";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将邮件检索结果发送到客户端
                          } else if (request == "Classification_of_query") {//分类别关键字查询用户收件箱
                              QString category = message.section("#",1,1);//排序关键字
                              QString keyword = message.section("#",2,2);//检索关键字
                              ui->textEdit->append("成功连接收件查询服务端！！");
                              /*服务端在数据库邮件表中根据用户名usr_name、关键字查询所有发送邮件并按keyword要求排序，返回拼接字符串结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=0) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=0) or (CS_people = :usr_name and e_preserve_statu=0)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' +num_mails;
                              QSqlQuery query2;
                              if (category=="时间") {
                                  ui->textEdit->append("query 时间");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=0) or (Cc_people =:usr_name and c_preserve_statu=0) or (CS_people =:usr_name and e_preserve_statu=0) )"
                                                         ") order by time;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString()+'#'+query2.value(3).toString();
                                  }
                              } else if (category=="好友名") {
                                  ui->textEdit->append("query 好友名");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=0) or (Cc_people =:usr_name and c_preserve_statu=0) or (CS_people =:usr_name and e_preserve_statu=0) )"
                                                         ") order by sender;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString()+'#'+query2.value(3).toString();
                                  }
                              } else if (category=="主题名") {
                                  ui->textEdit->append("query 主题名");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=0) or (Cc_people =:usr_name and c_preserve_statu=0) or (CS_people =:usr_name and e_preserve_statu=0) )"
                                                         ") order by title;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString()+'#'+query2.value(3).toString();
                                  }
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#1#发件人1#主题1#正文1";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将相关邮件排序检索结果发送到客户端
                          } else if (request == "rubbishMail") {//窗口初始化查询用户垃圾箱
                              /*服务端在数据库邮件表中根据用户名usr_name查询所有垃圾邮件，返回结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=1 ) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=1) or (CS_people = :usr_name and e_preserve_statu=1)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string+'#'+num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where (recipient=:usr_name and r_preserve_statu=1 ) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=1) or (CS_people = :usr_name and e_preserve_statu=1)"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString()+ '#' + query2.value(1).toString()+ '#' + query2.value(2).toString();
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#2#发件人1#主题1#正文1#发件人2#主题2#正文2";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将所有未排序邮件检索结果发送到客户端
                          } else if (request == "recover") {//恢复删除邮件
                              QString sender = message.section("#",1,1);//发送人
                              QString theme = message.section("#",2,2);//检索关键字
                              //恢复垃圾邮件
                              //作为收件人
                              QSqlQuery querya;
                              querya.prepare(QString("update tb_mails set r_preserve_statu=0 where recipient=:usr_name and sender=:sender and title=:theme"));
                              querya.bindValue(":usr_name",usr_name);
                              querya.bindValue(":sender",sender);
                              querya.bindValue(":theme",theme);
                              if (!querya.exec()) {
                                  ui->textEdit->append(querya.lastError().text());}
                              else {
                                  ui->textEdit->append("querya success!");
                              }
                              ui->textEdit->append(querya.lastError().text());
                              //作为抄送人,状态0为储存
                              QSqlQuery queryb;
                              queryb.prepare(QString("update tb_mails set c_preserve_statu=0 where Cc_people=:usr_name and sender=:sender and title=:theme"));
                              queryb.bindValue(":usr_name",usr_name);
                              queryb.bindValue(":sender",sender);
                              queryb.bindValue(":theme",theme);
                              if (!queryb.exec()) {
                                  ui->textEdit->append(queryb.lastError().text());}
                              else {
                                  ui->textEdit->append("queryb success!");
                              }
                              ui->textEdit->append(queryb.lastError().text());
                              //作为密送人
                              QSqlQuery queryc;
                              queryc.prepare(QString("update tb_mails set e_preserve_statu=0 where CS_people=:usr_name and sender=:sender and title=:theme"));
                              queryc.bindValue(":usr_name",usr_name);
                              queryc.bindValue(":sender",sender);
                              queryc.bindValue(":theme",theme);
                              if (!queryc.exec()) {
                                  ui->textEdit->append(queryc.lastError().text());}
                              else{
                                  ui->textEdit->append("queryc success!");
                              }
                              ui->textEdit->append(queryc.lastError().text());
                              //返回未排序结果
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=1) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=1) or (CS_people = :usr_name and e_preserve_statu=1)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' + num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where(recipient=:usr_name and r_preserve_statu =1) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu =1) or (CS_people = :usr_name and e_preserve_statu =1) "));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                  return_string = return_string + '#' + query2.value(0).toString()+ '#' + query2.value(1).toString()+ '#' + query2.value(2).toString();
                              }
                              /*根据sender 和 theme 在数据库中恢复被删除的邮件，成功返回“success#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              //QString return_string = "recoverSuccess#1#恢复后的发件人1#恢复后的主题1#恢复后的正文1";//测试返回成功
                              ui->textEdit->append(return_string);
                              tcpSocket->write(return_string.toUtf8().data());//将恢复结果发送到客户端
                          } else if (request == "rubbishDelete") {//彻底删除邮件
                              QString sender = message.section("#",1,1);//排序关键字
                              QString theme = message.section("#",2,2);//检索关键字
                              //恢复垃圾邮件
                              //作为收件人
                              QSqlQuery querya;
                              querya.prepare(QString("update tb_mails set r_preserve_statu=2 where recipient=:usr_name and sender=:sender and title=:theme"));
                              querya.bindValue(":usr_name",usr_name);
                              querya.bindValue(":sender",sender);
                              querya.bindValue(":theme",theme);
                              if (!querya.exec()) {
                                  ui->textEdit->append(querya.lastError().text());
                              } else{
                                  ui->textEdit->append("querya success!");
                              }
                              ui->textEdit->append(querya.lastError().text());
                              //作为抄送人
                              QSqlQuery queryb;
                              queryb.prepare(QString("update tb_mails set c_preserve_statu=2 where Cc_people=:usr_name and sender=:sender and title=:theme"));
                              queryb.bindValue(":usr_name",usr_name);
                              queryb.bindValue(":sender",sender);
                              queryb.bindValue(":theme",theme);
                              if (!queryb.exec()) {
                                  ui->textEdit->append(queryb.lastError().text());
                              } else {
                                  ui->textEdit->append("queryb success!");
                              }
                              ui->textEdit->append(queryb.lastError().text());
                              //作为密送人
                              QSqlQuery queryc;
                              queryc.prepare(QString("update tb_mails set e_preserve_statu=2 where CS_people=:usr_name and sender=:sender and title=:theme"));
                              queryc.bindValue(":usr_name",usr_name);
                              queryc.bindValue(":sender",sender);
                              queryc.bindValue(":theme",theme);
                              if (!queryc.exec()) {
                                  ui->textEdit->append(queryc.lastError().text());
                              } else {
                                  ui->textEdit->append("queryc success!");
                              }
                              //查询数量
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where sender=:usr_name and (recipient=:usr_name and r_preserve_statu =1) or "
                                                     "(Cc_people = :usr_name c_preserve_statu =1) or (CS_people = :usr_name and e_preserve_statu =1)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "recoverSuccess";
                              return_string = return_string+'#'+num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where (recipient=:usr_name and r_preserve_statu =1) or "
                                                     "(Cc_people = :usr_name c_preserve_statu =1) or (CS_people = :usr_name and e_preserve_statu =1) "));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while(query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString()+ '#' + query2.value(1).toString()+ '#' + query2.value(2).toString();
                              }
                              ui->textEdit->append(return_string);
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#2#发件人1#主题1#正文1#发件人2#主题2#正文2";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将所有未排序邮件检索结果发送到客户端
                          } else if (request == "Classification_of_query_rubbish") {//分类别关键字查询用户垃圾箱
                              QString category = message.section("#",1,1);//排序关键字
                              QString keyword = message.section("#",2,2);//检索关键字
                              ui->textEdit->append("成功连接垃圾邮件查询服务端！！");
                              /*服务端在数据库邮件表中根据用户名usr_name、关键字查询所有垃圾邮件并按keyword要求排序，返回拼接字符串结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where (recipient=:usr_name and r_preserve_statu=1) or "
                                                     "(Cc_people = :usr_name and c_preserve_statu=1) or (CS_people = :usr_name and e_preserve_statu=1)"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' +num_mails;
                              QSqlQuery query2;
                              if (category=="时间") {
                                  ui->textEdit->append("query 时间");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=1) or (Cc_people =:usr_name and c_preserve_statu=1) or (CS_people =:usr_name and e_preserve_statu=1) )"
                                                         ") order by time;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              } else if (category=="收件人") {
                                  ui->textEdit->append("query 收件人");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=1) or (Cc_people =:usr_name and c_preserve_statu=1) or (CS_people =:usr_name and e_preserve_statu=1) )"
                                                         ") order by recipient;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              } else if (category=="主题") {
                                  ui->textEdit->append("query 主题");
                                  query2.prepare(QString("select sender,title,MainText from tb_mails where("
                                                             "((recipient=:usr_name and r_preserve_statu=1) or (Cc_people =:usr_name and c_preserve_statu=1) or (CS_people =:usr_name and e_preserve_statu=1) )"
                                                         ") order by title;"));
                                  query2.bindValue(":usr_name",usr_name);
                                  query2.bindValue(":keyword",keyword);
                                  query2.exec();
                                  /*查询结果qstring拼接格式：“#return_mail2#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                                  while(query2.next()) {
                                     return_string = return_string + '#' + query2.value(0).toString()+'#'+query2.value(1).toString()+'#'+query2.value(2).toString();
                                  }
                              }
                              ui->textEdit->append(query2.lastError().text());
                              //QString return_string = "return_mail#1#发件人1#主题1#正文1";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将相关邮件排序检索结果发送到客户端
                          } else if (request == "draftMail") {//窗口初始化查询用户草稿箱
                              /*服务端在数据库邮件表中根据用户名usr_name查询所有草稿邮件，返回结果*/
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where sender =:usr_name and s_preserve_statu=0 and send_statu=0"));
                              query1.bindValue(":usr_name",usr_name);
                              query1.exec();
                              query1.next();
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string +'#'+num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where sender =:usr_name and s_preserve_statu=0 and send_statu=0"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              while(query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString()+ '#' + query2.value(1).toString()+ '#' + query2.value(2).toString();
                              }
                              ui->textEdit->append(query2.lastError().text());
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              //QString return_string = "return_mail#2#发件人1#主题1#正文1#发件人2#主题2#正文2";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将所有未排序邮件检索结果发送到客户端
                          } else if (request == "draftDelete") {//删除草稿
                              QString recipient = message.section("#",1,1);//获取邮件发件人
                              QString theme = message.section("#",2,2);//获取邮件主题
                              ui->textEdit->append("成功进入删除草稿服务端！！");
                              /*服务端先在数据库中根据收件人recieve和主题theme将对应草稿移至垃圾箱，再同上用关键字查询用户草稿箱，返回邮件查询结果*/
                              //移入垃圾箱
                              QSqlQuery query;
                              query.prepare(QString("update tb_mails set s_preserve_statu=1 where recipient=:recipient and sender=:usr_name and title=:theme and s_preserve_statu=0"));
                              query.bindValue(":recipient",recipient);
                              query.bindValue(":usr_name",usr_name);
                              query.bindValue(":theme",theme);
                              query.exec();
                              ui->textEdit->append(query.lastError().text());
                              //返回未排序结果
                              QSqlQuery query1;
                              query1.prepare(QString("select count(1) from tb_mails where sender=:usr_name and s_preserve_statu=0"));
                              query1.bindValue(":usr_namr",usr_name);
                              query1.exec();
                              query1.next();
                              ui->textEdit->append(query1.lastError().text());
                              QString num_mails = query1.value(0).toString();//邮件数量
                              QString return_string = "return_mail";
                              return_string = return_string + '#' + num_mails;
                              QSqlQuery query2;
                              query2.prepare(QString("select sender,title,MainText from tb_mails where send_statu = 0 and sender=:usr_name and s_preserve_statu=0"));
                              query2.bindValue(":usr_name",usr_name);
                              query2.exec();
                              /*查询结果qstring拼接格式：“#return_mail#邮件数量#发件人1#主题1#正文1#发件人2#主题2#正文2#······”*/
                              while (query2.next()) {
                                 return_string = return_string + '#' + query2.value(0).toString() + '#' + query2.value(1).toString() + '#' + query2.value(2).toString();
                              }
                              //QString return_string = "return_mail#1#删除后的发件人1#删除后的主题1#删除后的正文1";//测试邮件拼接
                              tcpSocket->write(return_string.toUtf8().data());//将所有未排序草稿检索结果发送到客户端
                          }
                      });
            });
}

//连接数据库
void ServerWidget::openDatabase(){
    QSqlDatabase db = QSqlDatabase::addDatabase("QMYSQL");
    db.setHostName("localhost");//数据库服务器IP
    db.setUserName("root");//数据库用户名
    db.setPassword("123456");//密码
    db.setDatabaseName("NeuMail");//使用的数据库名

    //打开数据库
    if(!db.open())//数据库打开失败
    {
        ui->textEdit->append(db.lastError().text());
        return;
    } else {
        //数据库打开失败
        ui->textEdit->append("database connect success");
    }
}

//登录
void ServerWidget::login(QString message){
    //提取用户名密码
    usr_name = message.section("#",1,1);
    QString usr_password = message.section("#",2,2);
    /*服务端在数据库中验证用户名密码，获得返回值*/
    QSqlQuery query;
    query.prepare(QString("select passwd from tb_users where user_id = ? "));
    query.addBindValue(usr_name);
    query.exec();
    query.next();
    QString true_password = query.value(0).toString();
    QString return_sign;//返回值
    if(true_password == usr_password)
    {
        return_sign = "success";
    }
    ui->textEdit->append(return_sign);
    tcpSocket->write(return_sign.toUtf8().data());//将返回值发送到客户端，判断是否发送成功
}

//注册
void ServerWidget::regist(QString message){
    //接受用户名、密码、保密问题、答案并写入数据库
    usr_name = message.section("#",1,1);
    QString usr_password = message.section("#",2,2);
    QString question = message.section("#",3,3);
    QString answer = message.section("#",4,4);
    /*服务端在数据库中写入用户名、密码、保密问题、答案，若无重复用户名，写入数据成功，获得返回值success*/
    QSqlQuery query;
    QString return_string;
    query.prepare(QString("select passwd from tb_users where user_id = ? "));
    query.addBindValue(usr_name);
    query.exec();
    if(!query.next()){
        QString sql ="insert into tb_users(user_id,passwd,question,answer) "
                     "values(:user_id,:passwd,:question,:answer)";
        query.prepare(sql);
        query.bindValue(":user_id",usr_name);
        query.bindValue(":passwd",usr_password);
        query.bindValue(":question",question);
        query.bindValue(":answer",answer);
        ui->textEdit->append("query success!");
        return_string = "success";
    }
    tcpSocket->write(return_string.toUtf8().data());//将返回值发送到客户端，判断是否发送成功
}

//查看密保问题
void ServerWidget::viewQuestion(QString message){
    usr_name = message.section("#",1,1);
    /*服务端在数据库中检索用户名对应的保密问题，需获得返回值*/
    QString return_sign;
    QSqlQuery query;
    query.prepare(QString("select question from tb_users where user_id = ? "));
    query.addBindValue(usr_name);
    query.exec();
    query.next();
    return_sign = query.value(0).toString();
    QString request = "question";
    QString return_string = request + '#' + return_sign;
    ui->textEdit->append(return_string);
    tcpSocket->write(return_string.toUtf8().data());//将问题发送到客户端
}

//更改密码
void ServerWidget::change(QString message){
    usr_name = message.section("#",1,1);
    QString answer = message.section("#",2,2);
    QString usr_password = message.section("#",3,3);
    /*服务端在数据库中检索用户名对应的保密问题答案，若相同，修改密码并返回success*/
    QSqlQuery query;
    query.prepare(QString("select answer from tb_users where user_id = ? "));
    query.addBindValue(usr_name);
    query.exec();
    query.next();
    QString true_answer = query.value(0).toString();
    QString return_sign;
    if(true_answer==answer) {
        QSqlQuery queryin;
        queryin.prepare(QString("update tb_users set passwd=? where user_id =?"));
        queryin.addBindValue(usr_password);
        queryin.addBindValue(usr_name);
        queryin.exec();
        return_sign = "success";
    }
    QString request = "result";
    QString return_string = request + '#' + return_sign;
    ui->textEdit->append(return_string);
    tcpSocket->write(return_string.toUtf8().data());//将问题答案检索结果发送到客户端
}

//发送邮件
void ServerWidget::sendtext(QString message){
    QString recipient = message.section("#",1,1);
    QString time = message.section("#",2,2);
    QString theme = message.section("#",3,3);
    QString MainText = message.section("#",4,4);
    QString flag_attach = message.section("#",5,5);
    QString Cc_people = message.section("#",6,6);
    QString CS_people = message.section("#",7,7);
    QString FileName = message.section("#",8,8);
    if (flag_attach=="true") {
      ui->textEdit->append("receiving attach");
    }
    /*recipient 收件人， MainTextCc_people 抄送人， CS_people 密送人， theme 主题， MainText 邮件正文*/
    /*服务端在数据库邮件表中写入已发送邮件信息*/
    QSqlQuery query;
    QString sql ="insert into tb_mails (sender,recipient,time,Cc_people,CS_people,title,file,MainText,send_statu)"
                     " values (:usr_name,:recipient,:time,:Cc_people,:CS_people,:theme,:file,:MainText,:send_statu)";
    query.prepare(sql);
    query.bindValue(":file",FileName);
    query.bindValue(":usr_name",usr_name);
    query.bindValue(":recipient",recipient);
    query.bindValue(":Cc_people",Cc_people);
    query.bindValue(":CS_people",CS_people);
    query.bindValue(":time",time);
    query.bindValue(":theme",theme);
    query.bindValue(":MainText",MainText);
    query.bindValue(":send_statu",1);
    query.exec();
}

//发送附件
void ServerWidget::download(QString message){
    ui->textEdit->append("send file");
    QString sendName = message.section("#",1,1);

    QString ip = MY_IP;
    tcpSocket_attach->connectToHost(QHostAddress(ip),8889);

    SfileSize = 0;
    sendSize = 0;

    Spath = d.currentPath() + "/attach/" + sendName;
    ui->textEdit->append(Spath);
    timer.start(200);
}

//保存草稿
void ServerWidget::saveDraft(QString message){
    QString recipient = message.section("#",1,1);
    QString theme = message.section("#",2,2);
    QString MainText = message.section("#",3,3);
    QString Cc_people = message.section("#",4,4);
    QString CS_people = message.section("#",5,5);

    /*recipient 收件人， MainTextCc_people 抄送人， CS_people 密送人， theme 主题， MainText 邮件正文*/
    /*服务端在数据库邮件表中写入草稿邮件信息*/
    QSqlQuery query;
    QString sql ="insert into tb_mails (sender,recipient,title,MainText,Cc_people,CS_people,send_statu)"
                     " values (:sender,:recipient,:theme,:MainText,:Cc_people,:CS_people,:send_statu)";
    query.prepare(sql);
    query.bindValue(":sender",usr_name);
    query.bindValue(":recipient",recipient);
    query.bindValue(":Cc_people",Cc_people);
    query.bindValue(":CS_people",CS_people);
    query.bindValue(":theme",theme);
    query.bindValue(":MainText",MainText);
    query.bindValue(":send_statu",0);
    query.exec();
}

//查询未读邮件
void ServerWidget::unread(){
    /*服务端在数据库邮件表中计数用户usr_name未读邮件，返回结果*/
    QString return_string = "0";
    QSqlQuery query;
    query.prepare(QString("select count(1) from tb_mails where (recipient= :usr_name and r_read_statu=0 and r_preserve_statu=0) or (Cc_people =:usr_name and c_read_statu=0 and c_preserve_statu=0) or (CS_people = :usr_name and e_read_statu=0 and e_preserve_statu=0)"));
    query.bindValue(":usr_name",usr_name);
    query.exec();
    query.next();
    return_string = query.value(0).toString();
    tcpSocket->write(return_string.toUtf8().data());
}

void ServerWidget::newClient(){
    ui->textEdit->append("receive file");
    QString ip = tcpSocket_attach->peerAddress().toString();
    qint16 port = tcpSocket_attach->peerPort();
    QString temp = QString("[%1:%2]:成功连接").arg(ip).arg(port);
    ui->textEdit->append(temp);
    tcpSocket_attach = tcpServer_attach.nextPendingConnection();
    connect(tcpSocket_attach,&QTcpSocket::readyRead,this,&ServerWidget::readData);
    fileSize = 0;
    recSize = 0;
}

void ServerWidget::readData(){
    if (fileSize == 0) {
        QByteArray array = tcpSocket_attach->readAll();
        QDataStream stream(&array,QIODevice::ReadOnly);//把套接字与数据流绑定
        stream >> fileSize >> fileName;//获取文件大小、文件名
        file.setFileName(path+fileName);
        file.open(QIODevice::WriteOnly);
    }
    if (recSize < fileSize) {
        QByteArray array = tcpSocket_attach->readAll();
        file.write(array);
        recSize += array.size();
    }
    if (recSize == fileSize) {
        ui->textEdit->append("attach is downloaded");
        file.close();
    }
}

ServerWidget::~ServerWidget() {
    delete ui;
}

void ServerWidget::send_head() {
    ui->textEdit->append("start send");
    QFileInfo info(Spath);
    SfileName = info.fileName();
    SfileSize = info.size();
    qDebug() << d.currentPath();
    Sfile.setFileName(Spath);
    Sfile.open(QIODevice::ReadOnly);
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream << SfileSize << SfileName;
    tcpSocket_attach->write(array);
}

void ServerWidget::send_file() {
    connect(&timer,&QTimer::timeout,
            [=](){
        timer.stop();
        sendData();
    });
}

void ServerWidget::sendData(){
    if (sendSize < SfileSize) {
        QByteArray array = Sfile.read(1024*10);
        tcpSocket_attach->write(array);
        sendSize += array.size();
        timer.start(200);
    }
    if (sendSize == SfileSize) {
        ui->textEdit->append("send success");
        Sfile.close();
    }
}
