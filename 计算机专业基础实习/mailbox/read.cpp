#include "read.h"
#include "ui_read.h"

read::read(QWidget *parent,QString a,QString b,QString c) :
    QDialog(parent),
    rsender(a),
    rMailTheme(b),
    rMainText(c),
    ui(new Ui::read)

{
    ui->setupUi(this);

    setFixedSize(565, 474);

    ui->label_3->setText(rsender);
    ui->label_4->setText(rMailTheme);
    ui->textBrowser->insertPlainText(rMainText);


}

read::~read()
{
    delete ui;
}


void read::on_reply_clicked()
{
    Write *mw = new Write(this,rsender,rMailTheme,rMainText);
    mw->receiver.insert(0,rsender);
    mw->show();
}
