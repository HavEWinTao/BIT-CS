#ifndef ALREADY_H
#define ALREADY_H

#include <QMainWindow>
#include "read.h"
#include "readwithfile.h"

namespace Ui {
class already;
}

class already : public QMainWindow
{
    Q_OBJECT

public:

    explicit already(QWidget *parent = nullptr);

    ~already();

    void openRead();

    QTcpSocket *tcpSocket;

private slots:

    void on_pushButton_3_clicked();

    void on_inbox_clicked();

    void on_outbox_clicked();

    void on_draft_clicked();

    void on_trash_clicked();

private:

    Ui::already *ui;
};

#endif // ALREADY_H
