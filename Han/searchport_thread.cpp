#include "searchport_thread.h"

SearchPort_Thread::SearchPort_Thread(QObject *parent) : QThread(parent)
{
    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &SearchPort_Thread::searchSerialPorts);
    timer->start(500);
}

void SearchPort_Thread::searchSerialPorts()//定时扫描
{
    QList<QSerialPortInfo> portInfos = QSerialPortInfo::availablePorts();
    QStringList ports;
    for (const QSerialPortInfo &info : portInfos) {
        ports.append(info.portName());
    }
    emit serialPortsFound(ports);
}

void SearchPort_Thread::run()
{
    QThread::usleep(50);
}
