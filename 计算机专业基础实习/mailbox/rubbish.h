#ifndef RUBBISH_H
#define RUBBISH_H

#include "all.h"
#include "read.h"
#include "readwithfile.h"

namespace Ui {
class rubbish;
}

class rubbish : public QMainWindow
{
    Q_OBJECT

public:

    explicit rubbish(QWidget *parent = nullptr);

    ~rubbish();

    void openRead();

    QTcpSocket *tcpSocket;

private slots:

    void on_pushButton_3_clicked();

    void on_pushButton_12_clicked();

    void on_outbox_clicked();

    void on_draft_clicked();

    void on_trash_clicked();

private:

    Ui::rubbish *ui;

};

#endif // RUBBISH_H
