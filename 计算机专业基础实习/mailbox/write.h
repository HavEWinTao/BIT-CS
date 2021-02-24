#ifndef WRITE_H
#define WRITE_H

#include "all.h"
namespace Ui {
class Write;
}

class Write : public QMainWindow
{
    Q_OBJECT

public:

    //explicit Write(QWidget *parent = nullptr);

    Write(QWidget *parent=nullptr,QString a="",QString b="",QString c="");

    ~Write();

    QString theme;
    QString receiver;
    QString maintext;

private:
    Ui::Write *ui;

    QColorDialog *m_pColorDialog;//调色板显示类
    QPalette m_myPalette;//paltte对象
    int m_red;//颜色的三色，红绿蓝
    int m_green;
    int m_blue;

    QString filepath;
    QFile file;
    QString fileName;
    qint64 fileSize=0;
    qint64 sendSize=0;

    QTimer timer;

    QTcpSocket *tcpSocket_attach;

signals:

    void mySignal();

private slots:

    void on_reg_clicked();

    void on_fontComboBox_currentFontChanged(const QFont &f);

    void on_underscore_clicked();

    void on_bold_clicked();

    void on_italic_clicked();

    void on_fontSize_textChanged(const QString &arg1);

    void on_color_clicked();

    void slot_getColor(QColor);//获取调色板rgb值

    void slot_setBackColor();//设置背景色

    void on_pushButton_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_2_clicked();

    void on_attach_clicked();

    void send_file();

    void send_head();

    void send_data();

signals:

    void signal_changeBackColor();//改变背景色信号函数

public:

    QFont fon;
    bool flag1 = true;
    bool flag2 = true;
    bool flag3 = true;

protected:

    virtual void closeEvent(QCloseEvent *event);

    QTcpSocket *tcpSocket;//通信套接字
};

#endif // WRITE_H
