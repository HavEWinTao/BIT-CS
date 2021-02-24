#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "write.h"
#include "recieve.h"
#include "already.h"
#include "draft.h"
#include "rubbish.h"
#include "login.h"
#include "all.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    explicit MainWindow(QWidget *parent = nullptr);

    ~MainWindow();

    QString usr_name;

signals:

    void mySignal();

private slots:

    void on_reg_clicked();

    void timerUpdate(void);

    void on_goWrite_clicked();

    void on_goRecieve_clicked();

    void on_goRecieve2_clicked();

    void my_show_slot();

    void on_goAlready_clicked();

    void on_goDraft_clicked();

    void on_goRubbish_clicked();

    void on_NotRead_linkActivated(const QString &link);

private:

    Ui::MainWindow *ui;

    QTcpSocket *tcpSocket;

protected:

    virtual void closeEvent(QCloseEvent *event);

};

#endif // MAINWINDOW_H
