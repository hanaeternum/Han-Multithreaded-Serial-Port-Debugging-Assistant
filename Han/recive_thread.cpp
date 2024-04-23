#include "recive_thread.h"

Recive_Thread::Recive_Thread(QObject *parent) : QThread(parent)
{
    serialPort_ = new QSerialPort;
}

void Recive_Thread::setPortName(const QString &portName, QSerialPort *serialPort)
{
    portName_ = portName;
    serialPort_ = serialPort;
}

void Recive_Thread::run()
{
    while (isRunning()) {
        if (serialPort_->waitForReadyRead(20))
        {
            QByteArray data = serialPort_->readAll();
            emit send_Data_Received(data);
            data.clear();
        }
        usleep(50);
    }
}

