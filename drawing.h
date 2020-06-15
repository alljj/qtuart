#ifndef DRAWING_H
#define DRAWING_H

#include <QMainWindow>
#include <QtCharts>
#include <QValueAxis>
#include <QList>
#include <QPointF>
#include <QTimerEvent>
#include "qxtglobalshortcut.h"
#include <qxtglobal.h>
#include <QScreen>
class drawing : public QMainWindow
{
    Q_OBJECT
public:
    explicit drawing(QWidget *parent = nullptr);

signals:

public slots:
    void updata_value(QString data);
protected:

private:
    QList<QLineSeries *> m_serieslist;
    QValueAxis *axisX;
    QValueAxis *axisY;
    QChart *chart;
    QChartView *chartview;
    QLineSeries *series;
    QList<QPointF> mydata;
    int time_ck;
    int send_sec;
    bool is_start;
    int max_y;
    int max_x;
    QxtGlobalShortcut  *save;

protected:
    void timerEvent(QTimerEvent *event);


};

#endif // DRAWING_H
