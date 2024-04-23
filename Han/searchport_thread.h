#ifndef SEARCHPORT_THREAD_H
#define SEARCHPORT_THREAD_H

#include <QThread>
#include <QTimer>
#include <QStringList>
#include <QSerialPort>
#include <QSerialPortInfo>

class SearchPort_Thread : public QThread
{
    Q_OBJECT
public:
    explicit SearchPort_Thread(QObject *parent = nullptr);
    void searchSerialPorts();

protected:
    void run() override;

signals:
    void serialPortsFound(const QStringList &ports);
};

#endif
