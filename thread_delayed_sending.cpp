#include "thread_delayed_sending.h"
#include <QDebug>
#include <QMessageBox>
#pragma execution_character_set("utf-8")
thread_delayed_sending::thread_delayed_sending(QObject *parent) : QObject(parent)
{
//     connect(&serial,&QSerialPort::readyRead,this,&MainWindow::serialPort_readyRead);//读取串口消息的信号与槽


}

void thread_delayed_sending::init()
{
    qDebug()<<"子线程:"<<"初始化";
    serial = new QSerialPort;
    connect(serial,&QSerialPort::readyRead,this,&thread_delayed_sending::serialPort_readyRead);//读取串口消息的信号与槽
//    serial->setPortName("COM1");
//    qDebug()<<serial->open(QIODevice::ReadWrite);
    qDebug()<<"子线程号:"<<QThread::currentThread();
    emit init_finish();
}



void thread_delayed_sending::recieve_send_signal(QByteArray data)
{
    qDebug()<<"already_recieve";
   serial->write(data);
   qDebug()<<"子线程号:"<<QThread::currentThread();

}

void thread_delayed_sending::close_urt()
{
    qDebug()<<"子线程:"<<"关闭串口";
    serial->close();
    emit finish_send();
}

void thread_delayed_sending::config_mar(QString com, int band, int data, int check, int stop)
{
    qDebug()<<"接收成功";
    serial->close();
    serial->setPortName(com);
    serial->setBaudRate(band);
    //数据位
    switch(data)
    {
    case 0: serial->setDataBits(QSerialPort::Data8); break;
    case 1: serial->setDataBits(QSerialPort::Data7); break;
    case 2: serial->setDataBits(QSerialPort::Data6); break;
    case 3: serial->setDataBits(QSerialPort::Data5); break;
    default: break;
    }
    //设置奇偶校验
    switch(check)
    {
    case 0: serial->setParity(QSerialPort::NoParity); break;
    case 1: serial->setParity(QSerialPort::OddParity); break;
    case 2: serial->setParity(QSerialPort::EvenParity); break;
    default: break;
    }
    //设置停止位
    switch(stop)
    {
    case 0: serial->setStopBits(QSerialPort::OneStop); break;
    case 1: serial->setStopBits(QSerialPort::OneAndHalfStop); break;
    case 2: serial->setStopBits(QSerialPort::TwoStop); break;
    default: break;
    }
    //设置流控制
    serial->setFlowControl(QSerialPort::NoFlowControl);
    bool ok = serial->open(QIODevice::ReadWrite);
    emit  is_ok(ok);

}


void thread_delayed_sending::serialPort_readyRead()
{
   QByteArray buffer = serial->readAll();
   emit  recieve_data(buffer);
}
