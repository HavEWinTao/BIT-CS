#ifndef READ_H
#define READ_H

#include <QDialog>
#include "all.h"
#include "write.h"

namespace Ui {
class read;
}

class read : public QDialog
{
    Q_OBJECT

public:

    explicit read(QWidget *parent = nullptr,QString a = "default",QString b = "default",QString c = "default");

    ~read();

    QString rsender;//邮件发件人
    QString rMailTheme;//邮件主题
    QString rMainText;//邮件正文

private slots:

    void on_reply_clicked();

private:

    Ui::read *ui;

};

#endif // READ_H
