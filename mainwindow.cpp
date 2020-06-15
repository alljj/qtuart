#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include <QDebug>
#pragma execution_character_set("utf-8")
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow),
    is_click(false),
    is_run_thread(false)
{
    ui->setupUi(this);
    ui->textEdit_recieve->setReadOnly(true);
    this->setMaximumSize(442,518);
    this->setMaximumSize(442,518);
    ui->comboBox_com->clear();
    ui->send_data->setEnabled(false);
    send = new thread_delayed_sending;
    dr = new drawing;
    qDebug()<<"主线程号:"<<QThread::currentThread();
//    thread = new QThread(this);
    time_send = new QTimer(this);
    time_up = new QTimer(this);

    start_thread = new QTimer(this);//程序打开时 过1ms自动开启线程
    start_thread->start(1);
    serial.close();
    thread = new QThread;
    send->moveToThread(thread);
    thread->start();
    qDebug()<<"初始化线程，准备启动线程";

    connect(start_thread,&QTimer::timeout,send,&thread_delayed_sending::init);
    connect(send,&thread_delayed_sending::init_finish,[=]()
    {
            qDebug()<<"线程启动完毕";
            start_thread->stop();
            is_run_thread = true;
    }
      );

    select_baund(); //配置波特率选项
    select_stop();  //配置停止位选项
    select_data();  //配置数据位选项
    select_check(); //配置校验位选项


    connect(ui->radioButton_delayedsending,&QRadioButton::toggled,this,&MainWindow::radion_delayed_sending_status);//选择连续发送还是关闭连续发送的信号与槽

    connect(this,&MainWindow::close_uart,send,&thread_delayed_sending::close_urt);//关闭连续发送的信号与槽
    connect(time_send,&QTimer::timeout,this,&MainWindow::on_send_data_clicked);//定时发送数据的信号与槽
    connect(this,&MainWindow::delayed_sending,send,&thread_delayed_sending::recieve_send_signal);//收取数据信号与槽
    connect(this,&MainWindow::config_mas,send,&thread_delayed_sending::config_mar);
    connect(send,&thread_delayed_sending::is_ok,this,&MainWindow::message_box_ok);
    connect(send,&thread_delayed_sending::recieve_data,this,&MainWindow::serialPort_readyRead);

    connect(this,&MainWindow::send_data_to_draw,dr,&drawing::updata_value);

}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::select_baund()
{
    QStringList baundlist;
    baundlist<<"1382400"<<"921600"<<"460800"<<"256000"<<"230400"
            <<"128000"<<"115200"<<"76800"<<"57600"<<"43000"<<"38400"
           <<"19200"<<"14400"<<"9600"<<"4800"<<"2400"<<"1200";
    ui->comboBox_baund->addItems(baundlist);
}

void MainWindow::select_stop()
{
    QStringList stoplist;
    stoplist<<"1"<<"1.5"<<"2";
    ui->comboBox_stop->addItems(stoplist);
}

void MainWindow::select_data()
{
    QStringList datalist;
    datalist<<"8"<<"7"<<"6"<<"5";
    ui->comboBox_data->addItems(datalist);
}

void MainWindow::select_check()
{
    QStringList checklist;
    checklist<<"无校验"<<"奇校验"<<"偶校验";
    ui->comboBox_check->addItems(checklist);
}

void MainWindow::on_check_com_clicked()
{
    ui->comboBox_com->clear();
    comlist.clear();
    //自动检测电脑可用串口
    foreach(const QSerialPortInfo &info,QSerialPortInfo::availablePorts())
    {
        comlist<<info.portName();
        qDebug()<<comlist;
//        ui->comboBox_com->addItem(comlist);

    }
    qSort(comlist.begin(),comlist.end());
    ui->comboBox_com->addItems(comlist);

}

void MainWindow::on_open_com_clicked()
{
   if(ui->open_com->text() ==QString("打开串口"))
   {
       QString com = ui->comboBox_com->currentText();
       int baund = ui->comboBox_baund->currentText().toInt();
       int data = ui->comboBox_data->currentIndex();
       int check = ui->comboBox_check->currentIndex();
       int stop = ui->comboBox_stop->currentIndex();
       emit config_mas(com,baund,data,check,stop);

       ui->open_com->setText("关闭串口");
       is_click = true;
       ui->send_data->setEnabled(true);

   }
   else
   {

       ui->open_com->setText("打开串口");
       emit close_uart();
       is_click = false;
       ui->send_data->setEnabled(false);
   }

}

void MainWindow::on_send_data_clicked()
{

    QByteArray data = ui->textEdit_send->toPlainText().toUtf8();
    emit delayed_sending(data);

}



void MainWindow::serialPort_readyRead(QByteArray data)
{

   QString recv = ui->textEdit_recieve->toPlainText();
   recv += QString(data);
   //清空以前的显示
   ui->textEdit_recieve->clear();
   //重新显示
   ui->textEdit_recieve->append(QString("<font color=\"#00FF00\">%1</font>").arg(recv));
   send_data_to_draw(QString(data));

}



void MainWindow::on_comboBox_com_activated(const QString &arg1)
{
    if(is_click == true&&is_run_thread == true)
    {
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        emit config_mas(com,baund,data,check,stop);

    }
}

void MainWindow::on_comboBox_baund_activated(const QString &arg1)
{
    if(is_click == true&&is_run_thread == true)
    {
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        emit config_mas(com,baund,data,check,stop);
    }
}



void MainWindow::on_comboBox_data_activated(int index)
{
    if(is_click == true&&is_run_thread == true)
    {
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        emit config_mas(com,baund,data,check,stop);
    }

}

void MainWindow::on_comboBox_check_activated(int index)
{
    if(is_click == true&&is_run_thread == true)
    {
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        emit config_mas(com,baund,data,check,stop);
    }
}

void MainWindow::on_comboBox_stop_activated(int index)
{
    if(is_click == true&&is_run_thread == true)
    {
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        emit config_mas(com,baund,data,check,stop);
    }
}

void MainWindow::on_clear_data_clicked()
{
    ui->textEdit_recieve->clear();
}

void MainWindow::radion_delayed_sending_status(bool status)
{
    qDebug()<<status;
    if(status==true)
       {
       time_send->start(1000);
       }
    if(status ==false)
    {
        time_send->stop();
    }
}

void MainWindow::over_sending_thread()
{
//    send->quit();
//    send->wait();
}

void MainWindow::send_signal_slots()
{

        qDebug()<<"send signals";
        QByteArray data = ui->textEdit_send->toPlainText().toUtf8();
        emit delayed_sending(data);

}



void MainWindow::timerEvent(QTimerEvent *event)
{

}






void MainWindow::on_draw_pic_clicked()
{
  dr->show();
  //  this->hide();
}

void MainWindow::message_box_ok(bool ok)
{
    if(ok==false)
    {
                   QMessageBox::about(NULL,"提示","串口已打开!");
                   ui->open_com->setText("打开串口");
                   is_click= false;
                                      return;

    }
}
