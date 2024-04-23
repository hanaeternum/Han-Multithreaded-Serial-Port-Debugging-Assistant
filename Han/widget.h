#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QtDebug>
#include <QTextCodec>
#include <QSerialPort>
#include <QMessageBox>
#include <QSerialPortInfo>
#include <QMouseEvent>
#include <QWheelEvent>
#include "recive_thread.h"
#include "qcustomplot.h"
#include "searchport_thread.h"

QT_BEGIN_NAMESPACE
namespace Ui { class Widget; }
QT_END_NAMESPACE

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

    void updatePortsList(const QStringList &ports);
    void ui_update_recive();

private slots:

    void on_Switch_PushButton_clicked(bool checked);
    void fetch_Data_Received(const QByteArray &data);

    void on_Receive_Clear_clicked();

    void on_Receive_Hex_stateChanged(int arg1);

    void on_Send_clicked();

    void on_Transmit_Hex_stateChanged(int arg1);

    void on_Transmit_Clear_clicked();

    void on_Time_editingFinished();

    void Auto_Clear_Send();

    void on_Auto_Send_stateChanged(int arg1);

    void on_Auto_Clear_stateChanged(int arg1);

    void on_tabWidget_currentChanged(int index);

    void Oscilloscope_Update();

    void on_Clear_Oscilloscope_clicked();

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    Ui::Widget *ui;
    bool WheelLock = 0;
    bool ErrorLock = 0;//The first data received when opening the serial port may be very large.
    bool Hex_ReceiveLock = 0;
    bool Hex_TransmitLock = 0;
    bool Auto_ClearLock = 0;
    bool Auto_SendLock = 0;
    double count = 0;
    //QVector<double> Oscilloscope_Data;
    QCustomPlot *CustomPlot;
    QTimer *Auto_Timer;
    QTimer *Oscilloscope_Timer;
    QString out_Data;
    QString Recive_Data;
    std::vector<QString> Oscilloscope_Data;
    QStringList Old_Ports;
    QSerialPort *serialPort;
    Recive_Thread *recive_Thread;
    SearchPort_Thread* searchPort_Thread;
    QString stringToHex(const QString &str);
    QByteArray hexStringToByteArray(const QString &hexStr);
    double Max_Oscilloscope = -std::numeric_limits<double>::infinity();
    double Min_Oscilloscope = std::numeric_limits<double>::max(); // 初始化为最大值;
     double Sum_Oscilloscope = 0;
    double Average_Oscilloscope = 0;
};
#endif // WIDGET_H
