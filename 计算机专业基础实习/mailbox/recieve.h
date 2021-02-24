#ifndef RECIEVE_H
#define RECIEVE_H

#include "read.h"
#include "readwithfile.h"
#include "all.h"

namespace Ui {
class Recieve;
}

class Recieve : public QMainWindow
{
    Q_OBJECT

public:

    explicit Recieve(QWidget *parent = nullptr);

    ~Recieve();

private:

    Ui::Recieve *ui;

    QTcpSocket *tcpSocket;


signals:

    void mySignal();

    void sendData(QString);   //用来传递数据的信号

private slots:

    void on_reg_clicked();

    void on_pushButton_3_clicked();

    void on_outbox_clicked();

    void on_draft_clicked();

    void on_trash_clicked();

    void on_inbox_clicked();

protected:

    virtual void closeEvent(QCloseEvent *event);

};

#endif // RECIEVE_H
