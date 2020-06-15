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

    select_baund();
    select_stop();
    select_data();
    select_check();


    connect(ui->radioButton_delayedsending,&QRadioButton::toggled,this,&MainWindow::init);  //选择按钮启动多线程的信号与槽
    connect(&serial,&QSerialPort::readyRead,this,&MainWindow::serialPort_readyRead);//读取串口消息的信号与槽
    connect(ui->radioButton_delayedsending,&QRadioButton::toggled,this,&MainWindow::radion_delayed_sending_status);//选择连续发送还是关闭连续发送的信号与槽
//    connect(send,&thread_delayed_sending::isSendDone,this,&MainWindow::over_sending_thread);
    connect(this,&MainWindow::close_uart,send,&thread_delayed_sending::close_urt);//关闭连续发送的信号与槽
    connect(time_send,&QTimer::timeout,this,&MainWindow::send_signal_slots);//定时发送数据的信号与槽
    connect(this,&MainWindow::delayed_sending,send,&thread_delayed_sending::recieve_send_signal);//收取数据信号与槽
    connect(this,&MainWindow::config_mas,send,&thread_delayed_sending::config_mar);
    connect(this,&MainWindow::startThread,send,&thread_delayed_sending::init);//启动线程程序
    connect(send,&thread_delayed_sending::finish_send,this,&MainWindow::re_open);
    connect(this,&MainWindow::send_data_to_draw,dr,&drawing::updata_value);
//    connect(ui->comboBox_com,&QComboBox::currentTextChanged,send,&thread_delayed_sending::com_config);
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

       serial.setPortName(ui->comboBox_com->currentText());
       serial.setBaudRate(ui->comboBox_baund->currentText().toInt());
       //数据位
       switch(ui->comboBox_data->currentIndex())
       {
       case 0: serial.setDataBits(QSerialPort::Data8); break;
       case 1: serial.setDataBits(QSerialPort::Data7); break;
       case 2: serial.setDataBits(QSerialPort::Data6); break;
       case 3: serial.setDataBits(QSerialPort::Data5); break;
       default: break;
       }
       //设置奇偶校验
       switch(ui->comboBox_check->currentIndex())
       {
       case 0: serial.setParity(QSerialPort::NoParity); break;
       case 1: serial.setParity(QSerialPort::OddParity); break;
       case 2: serial.setParity(QSerialPort::EvenParity); break;
       default: break;
       }
       //设置停止位
       switch(ui->comboBox_stop->currentIndex())
       {
       case 0: serial.setStopBits(QSerialPort::OneStop); break;
       case 1: serial.setStopBits(QSerialPort::OneAndHalfStop); break;
       case 2: serial.setStopBits(QSerialPort::TwoStop); break;
       default: break;
       }
       //设置流控制
       serial.setFlowControl(QSerialPort::NoFlowControl);
       bool ok = serial.open(QIODevice::ReadWrite);
       if(ok==false)
        {
           QMessageBox::about(NULL,"提示","串口已打开!");
                              return;

        }
       ui->open_com->setText("关闭串口");
       is_click = true;
       ui->send_data->setEnabled(true);

   }
   else
   {
       serial.close();
       ui->open_com->setText("打开串口");
       is_click = false;
       ui->send_data->setEnabled(false);
   }

}

void MainWindow::on_send_data_clicked()
{

    QByteArray data = ui->textEdit_send->toPlainText().toUtf8();
    serial.write(data);

}



void MainWindow::serialPort_readyRead()
{
   QByteArray buffer = serial.readAll();
   QString recv = ui->textEdit_recieve->toPlainText();
   recv += QString(buffer);
   //清空以前的显示
   ui->textEdit_recieve->clear();
   //重新显示
   ui->textEdit_recieve->append(QString("<font color=\"#00FF00\">%1</font>").arg(recv));
   emit send_data_to_draw((QString)buffer);
//   ui->textEdit_recieve->append(recv);
}



void MainWindow::on_comboBox_com_activated(const QString &arg1)
{
    if(is_click == true)
    {
        qDebug()<<arg1;
        serial.close();
        serial.setPortName(arg1);
        bool ok =serial.open(QIODevice::ReadWrite);
        if(ok==false&&!is_run_thread)
        {
            QMessageBox::about(NULL,"提示","串口已打开!");
            ui->open_com->setText("打开串口");
             is_click = false;
             ui->send_data->setEnabled(false);
                               return;
        }
        if(is_run_thread == true)
        {
            thread_config_updata();
        }
    }
}

void MainWindow::on_comboBox_baund_activated(const QString &arg1)
{
    if(is_click == true)
    {
        qDebug()<<arg1;
        serial.close();
        serial.setBaudRate(arg1.toInt());
        bool ok =serial.open(QIODevice::ReadWrite);
        if(ok==false&&!is_run_thread)
        {
            QMessageBox::about(NULL,"提示","串口已打开!");
            ui->open_com->setText("打开串口");
             is_click = false;
             ui->send_data->setEnabled(false);
                               return;
        }
        if(is_run_thread == true)
        {
            thread_config_updata();
        }
    }
}



void MainWindow::on_comboBox_data_activated(int index)
{
    if(is_click == true)
    {
        qDebug()<<index;
        serial.close();
        switch(index)
        {
        case 0: serial.setDataBits(QSerialPort::Data8); break;
        case 1: serial.setDataBits(QSerialPort::Data7); break;
        case 2: serial.setDataBits(QSerialPort::Data6); break;
        case 3: serial.setDataBits(QSerialPort::Data5); break;
        default: break;
        }

        bool ok =serial.open(QIODevice::ReadWrite);
        if(ok==false&&!is_run_thread)
        {
            QMessageBox::about(NULL,"提示","串口已打开!");
            ui->open_com->setText("打开串口");
             is_click = false;
             ui->send_data->setEnabled(false);
                               return;
        }
        if(is_run_thread == true)
        {
            thread_config_updata();
        }
    }
}

void MainWindow::on_comboBox_check_activated(int index)
{
    if(is_click == true)
    {
        qDebug()<<index;
        serial.close();
        switch(index)
        {
        case 0: serial.setParity(QSerialPort::NoParity); break;
        case 1: serial.setParity(QSerialPort::OddParity); break;
        case 2: serial.setParity(QSerialPort::EvenParity); break;
        default: break;
        }

        bool ok =serial.open(QIODevice::ReadWrite);
        if(ok==false&&!is_run_thread)
        {
            QMessageBox::about(NULL,"提示","串口已打开!");
            ui->open_com->setText("打开串口");
            is_click = false;
            ui->send_data->setEnabled(false);
                               return;
        }
        if(is_run_thread == true)
        {
            thread_config_updata();
        }
    }
}

void MainWindow::on_comboBox_stop_activated(int index)
{
    if(is_click == true)
    {
        qDebug()<<index;
        serial.close();
        switch(index)
        {
        case 0: serial.setStopBits(QSerialPort::OneStop); break;
        case 1: serial.setStopBits(QSerialPort::OneAndHalfStop); break;
        case 2: serial.setStopBits(QSerialPort::TwoStop); break;
        default: break;
        }

        bool ok =serial.open(QIODevice::ReadWrite);
        if(ok==false&&!is_run_thread)
        {
            QMessageBox::about(NULL,"提示","串口已打开!");
            ui->open_com->setText("打开串口");
             is_click = false;
             ui->send_data->setEnabled(false);
                               return;
        }
        if(is_run_thread == true)
        {
            thread_config_updata();
        }
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
        time_up->start(10);
//        serial.clear();
       time_send->start(1000);
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

void MainWindow::init(bool status)
{
    if(status==true)
    {
        serial.close();
        thread = new QThread;
        send->moveToThread(thread);
        qDebug()<<"初始化";
        thread->start();
        emit startThread();

        qDebug()<<"给子线程传递当前参数";
        QString com = ui->comboBox_com->currentText();
        int baund = ui->comboBox_baund->currentText().toInt();
        int data = ui->comboBox_data->currentIndex();
        int check = ui->comboBox_check->currentIndex();
        int stop = ui->comboBox_stop->currentIndex();
        qDebug()<<"参数配置完成";
        emit config_mas(com,baund,data,check,stop);

        is_run_thread =true;

//        time_send->start(1000);

    }
    if(status == false)
    {

        emit close_uart();
//        delete send;
        time_send->stop();

    }
}

void MainWindow::re_open()
{
    qDebug()<<"重启计划";
    serial.setBaudRate(ui->comboBox_baund->currentText().toInt());
    serial.open(QIODevice::ReadWrite);

}

void MainWindow::timerEvent(QTimerEvent *event)
{

}

void MainWindow::thread_config_updata()
{
    emit close_uart();
    serial.close();
    qDebug()<<"更新配置";
    QString com = ui->comboBox_com->currentText();
    int baund = ui->comboBox_baund->currentText().toInt();
    int data = ui->comboBox_data->currentIndex();
    int check = ui->comboBox_check->currentIndex();
    int stop = ui->comboBox_stop->currentIndex();
    qDebug()<<"参数配置完成";
    emit config_mas(com,baund,data,check,stop);
}




void MainWindow::on_draw_pic_clicked()
{
  dr->show();
//  this->hide();
}
