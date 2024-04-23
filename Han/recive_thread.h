#ifndef RECIVE_THREAD_H
#define RECIVE_THREAD_H

#include <QThread>
#include <QDebug>
#include <QTextCodec>
#include <QSerialPort>
#include <QSerialPortInfo>

class Recive_Thread : public QThread
{
    Q_OBJECT
public:
    explicit Recive_Thread(QObject *parent = nullptr);

    QByteArray data;
    void setPortName(const QString &portName, QSerialPort *serialPort);

protected:

    void run() override;

signals:

    void send_Data_Received(const QByteArray &data);

private:
    QSerialPort *serialPort_;
    QString portName_;
};

#endif // RECIVE_THREAD_H
