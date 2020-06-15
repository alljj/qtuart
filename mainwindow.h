#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSerialPort>
#include <QSerialPortInfo>
#include "thread_delayed_sending.h"
#include <QTimerEvent>
#include <QTimer>

#include "drawing.h"
namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT


public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
     void select_baund();
     void select_stop();
     void select_data();
     void select_check();
     void thread_config_updata();
signals:
     void delayed_sending(QByteArray data);
     void startThread();
     void close_uart();
     void config_mas(QString com,int band,int data,int check,int stop);
     void send_data_to_draw(QString data);
private slots:
    void on_check_com_clicked();

    void on_open_com_clicked();

    void on_send_data_clicked();

    void serialPort_readyRead(QByteArray data);

    void on_comboBox_com_activated(const QString &arg1);

    void on_comboBox_baund_activated(const QString &arg1);

    void on_comboBox_data_activated(int index);

    void on_comboBox_check_activated(int index);

    void on_comboBox_stop_activated(int index);

    void on_clear_data_clicked();

    void radion_delayed_sending_status(bool status);

    void over_sending_thread();

    void send_signal_slots();

    void on_draw_pic_clicked();

    void message_box_ok(bool ok);

private:
    Ui::MainWindow *ui;
    QStringList comlist;
    bool is_click;
    thread_delayed_sending *send;
    QThread *thread;
    int timeid;
    QSerialPort serial;
    QTimer *time_send;
    QTimer *time_up;
    bool is_run_thread;
    drawing *dr;
    QTimer *start_thread;
protected:

          void timerEvent(QTimerEvent *event);
};

#endif // MAINWINDOW_H
