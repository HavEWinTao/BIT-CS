#ifndef DRAFT_H
#define DRAFT_H

#include <QMainWindow>
#include "read.h"
#include "readwithfile.h"

namespace Ui {
class draft;
}

class draft : public QMainWindow
{
    Q_OBJECT

public:

    explicit draft(QWidget *parent = nullptr);

    ~draft();

    void openRead();

    QTcpSocket *tcpSocket;

private slots:
    void on_inbox_clicked();

    void on_outbox_clicked();

    void on_draft_2_clicked();

    void on_trash_clicked();

private:

    Ui::draft *ui;

};

#endif // DRAFT_H
