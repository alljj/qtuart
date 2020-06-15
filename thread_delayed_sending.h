#ifndef THREAD_DELAYED_SENDING_H
#define THREAD_DELAYED_SENDING_H
#include <QThread>
#include <QObject>
#include <QSerialPort>
#include <QMainWindow>

class thread_delayed_sending : public QObject
{
    Q_OBJECT
public:
    explicit thread_delayed_sending(QObject *parent = 0);
    void init();
private:
       QSerialPort *serial;
signals:
       void isSendDone();
       void finish_send();
       void init_finish();
       void is_ok(bool is);
       void recieve_data(QByteArray buffer);
protected:
//       void run();
//         void timerEvent(QTimerEvent *event);
public slots:
       void recieve_send_signal(QByteArray data);
       void close_urt();
       void config_mar(QString com,int band,int data,int check,int stop);
       void serialPort_readyRead();
};

#endif // THREAD_DELAYED_SENDING_H
