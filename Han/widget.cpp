#include "widget.h"
#include "ui_widget.h"
#include "recive_thread.h"
#include "searchport_thread.h"

Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);

    this->setWindowTitle("Han SerialPort");

    ui->Receive->setAcceptRichText(true);

    CustomPlot = ui->CustomPlot;
    CustomPlot->addGraph();
    CustomPlot->xAxis->setLabel("X轴");
    CustomPlot->yAxis->setLabel("Y轴");
    serialPort = new QSerialPort(this);

    searchPort_Thread = new SearchPort_Thread;
    searchPort_Thread->start();

    recive_Thread = new Recive_Thread;
    recive_Thread->start();

    Auto_Timer = new QTimer(this);
    connect(Auto_Timer, &QTimer::timeout, this, &Widget::Auto_Clear_Send);

    Oscilloscope_Timer = new QTimer(this);
    connect(Oscilloscope_Timer, &QTimer::timeout, this, &Widget::Oscilloscope_Update);

    QTimer *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::ui_update_recive);
    timer->start(5);

    connect(searchPort_Thread, &SearchPort_Thread::serialPortsFound, this, &Widget::updatePortsList);//子线程(SearchPort_Thread)自动搜索串口
    connect(recive_Thread, &Recive_Thread::send_Data_Received, this, &Widget::fetch_Data_Received);
    //connect(serialPort, &QSerialPort::readyRead, recive_Thread, &Recive_Thread::on_Data_Received);
}

Widget::~Widget()
{
    delete ui;
}

void Widget::updatePortsList(const QStringList &ports)
{
    if (Old_Ports != ports){
        ui->Port->clear();
        for (const QString &port : ports) {
            ui->Port->addItem(port);
        }
    }
    Old_Ports = ports;
}

void Widget::on_Switch_PushButton_clicked(bool checked)
{
    if (checked){
        serialPort->setPortName(ui->Port->currentText());
        serialPort->setBaudRate(ui->Baudrate->currentText().toInt());
        switch (ui->Databit->currentText().toInt()) {
        case 5:
            serialPort->setDataBits(QSerialPort::Data5);
            break;
        case 6:
            serialPort->setDataBits(QSerialPort::Data6);
            break;
        case 7:
            serialPort->setDataBits(QSerialPort::Data7);
            break;
        case 8:
            serialPort->setDataBits(QSerialPort::Data8);
            break;
        default:
            break;
        }
        switch (ui->Paritybit->currentIndex()) {
        case 0:
            serialPort->setParity(QSerialPort::NoParity);
            break;
        case 1:
            serialPort->setParity(QSerialPort::EvenParity);
            break;
        case 2:
            serialPort->setParity(QSerialPort::OddParity);
            break;
        case 3:
            serialPort->setParity(QSerialPort::SpaceParity);
            break;
        case 4:
            serialPort->setParity(QSerialPort::MarkParity);
            break;
        default:
            break;
        }
        switch (ui->Stopbit->currentText().toInt()) {
        case 1:
            serialPort->setStopBits(QSerialPort::OneStop);
            break;
        case 2:
            serialPort->setStopBits(QSerialPort::TwoStop);
            break;
        case 3:
            serialPort->setStopBits(QSerialPort::OneAndHalfStop);
            break;
        default:
            break;
        }
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
        if (!serialPort->open(QIODevice::ReadWrite)){
            QMessageBox::about(NULL, "错误", "串口无法打开！可能串口已经被占用！");
            return;
        }
        else{
            ui->Port->setEnabled(false);
            ui->Baudrate->setEnabled(false);
            ui->Databit->setEnabled(false);
            ui->Paritybit->setEnabled(false);
            ui->Stopbit->setEnabled(false);
            ui->Switch_PushButton->setText("Close");
            ui->Switch_PushButton->setStyleSheet("QPushButton { background-color: grey; color: white; }");
            emit recive_Thread->setPortName(serialPort->portName(), serialPort);
            ErrorLock = 1;
        }
    }
    else{
        serialPort->close();
        ui->Port->setEnabled(true);
        ui->Baudrate->setEnabled(true);
        ui->Databit->setEnabled(true);
        ui->Paritybit->setEnabled(true);
        ui->Stopbit->setEnabled(true);
        ui->Switch_PushButton->setText("Open");
        ui->Switch_PushButton->setStyleSheet("QPushButton { background-color: grey; color: black; }");
        ErrorLock = 0;
    }
}

void Widget::ui_update_recive()
{
    if (!Recive_Data.isEmpty())
    {
        if (Hex_ReceiveLock == 0)
        {
            ui->Receive->insertPlainText(Recive_Data);
            Recive_Data.clear();
            if (!WheelLock)
            {
                QTextDocument *document = ui->Receive->document();
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::End);
                ui->Receive->setTextCursor(cursor);
            }
        }
        else
        {
            ui->Receive->insertPlainText(stringToHex(Recive_Data));
            Recive_Data.clear();
            if (!WheelLock)
            {
                QTextDocument *document = ui->Receive->document();
                QTextCursor cursor(document);
                cursor.movePosition(QTextCursor::End);
                ui->Receive->setTextCursor(cursor);
            }
        }
    }
}

void Widget::fetch_Data_Received(const QByteArray &data)
{
    if (ErrorLock)
    {
        ErrorLock = 0;
    }
    else
    {
        out_Data = QString::fromLocal8Bit(data);
        Recive_Data.append(out_Data);
        Oscilloscope_Data.push_back(out_Data);
        qDebug() << Oscilloscope_Data << endl;
        out_Data.clear();
    }

}

void Widget::on_Receive_Clear_clicked()
{
    ui->Receive->clear();
}

void Widget::wheelEvent(QWheelEvent *event)
{
    Q_UNUSED(event);
    qDebug() << WheelLock << endl;
    if (serialPort->isOpen())
    {
        WheelLock = 1;
    }
    else
    {
        WheelLock = 0;
    }
}

QString Widget::stringToHex(const QString &str) {
    QString hexString;
    QByteArray ba = str.toUtf8(); // 将 QString 转换为 UTF-8 编码的 QByteArray
    for (int i = 0; i < ba.length(); ++i) {
        unsigned char byte = ba.at(i);
        // 格式化每个字节为两位的16进制字符串，并用空格分隔
        hexString += QString("%1").arg(byte, 2, 16, QLatin1Char('0'));
        if (i < ba.length() - 1) { // 在不是最后一个字节时添加空格
            hexString += " ";
        }
    }
    return hexString;
}

QByteArray Widget::hexStringToByteArray(const QString &hexStr)
{
    QStringList hexList = hexStr.split(" ", QString::SkipEmptyParts);
    QByteArray byteArray;

    // 遍历分割后的十六进制数值列表
    for (const QString &hex : hexList) {
        // 将每个十六进制数值转换为字节，并添加到字节数组中
        bool ok;
        quint8 byte = hex.toUShort(&ok, 16); // 从字符串转换为无符号短整型
        if (ok) {
            byteArray.append(byte);
        } else {
            // 如果转换失败，可以记录错误或抛出异常
            qWarning() << "Invalid hex value:" << hex;
        }
    }

    return byteArray;
}

void Widget::on_Receive_Hex_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        Hex_ReceiveLock = 0;
    }
    else
    {
        Hex_ReceiveLock = 1;
    }
}

void Widget::on_Send_clicked()
{
    QString data = ui->Tranmit->toPlainText();
    if (Hex_TransmitLock == 0)
    {
        serialPort->write(hexStringToByteArray(data));
    }
    else
    {
        serialPort->write(stringToHex(data).toUtf8());
    }
}

void Widget::on_Transmit_Hex_stateChanged(int arg1)
{
    QString data = ui->Tranmit->toPlainText();
    if (arg1 == 0)
    {
        ui->Tranmit->setPlainText(hexStringToByteArray(data));
        Hex_TransmitLock = 0;
    }
    else
    {
        ui->Tranmit->setPlainText(stringToHex(data));
        Hex_TransmitLock = 1;
    }
}

void Widget::on_Transmit_Clear_clicked()
{
    ui->Tranmit->clear();
}

void Widget::on_Time_editingFinished()
{
    if (ui->Time->text() != "")
    {
        Auto_Timer->start(ui->Time->text().toInt()*1000);
    }
    else
    {
        Auto_Timer->stop();
    }
}

void Widget::Auto_Clear_Send()
{
    if (Auto_SendLock)
    {
        QString data = ui->Tranmit->toPlainText();
        if (Hex_TransmitLock == 0)
        {
            serialPort->write(hexStringToByteArray(data));
        }
        else
        {
            serialPort->write(stringToHex(data).toUtf8());
        }
    }
    if (Auto_ClearLock)
    {
        ui->Receive->clear();
    }
}

void Widget::on_Auto_Send_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        Auto_SendLock = 0;
    }
    else
    {
        Auto_SendLock = 1;
    }
}

void Widget::on_Auto_Clear_stateChanged(int arg1)
{
    if (arg1 == 0)
    {
        Auto_ClearLock = 0;
    }
    else
    {
        Auto_ClearLock = 1;
    }
}

void Widget::on_tabWidget_currentChanged(int index)
{
    if (index == 0)
    {
        Oscilloscope_Timer->stop();
    }
    else
    {
        Oscilloscope_Timer->start(10);
    }
}

void Widget::Oscilloscope_Update()
{
    if (!Oscilloscope_Data.empty())
    {
        for (const QString &str : Oscilloscope_Data)
        {
            bool ok;
            double value = str.toDouble(&ok);
            if (ok)
            {
                count+=1;
                if (count > 3000)
                {
                    count = 0;
                }
                CustomPlot->graph(0)->addData(count, value);
                ui->CustomPlot->graph(0)->rescaleAxes();
                ui->CustomPlot->replot();

                Sum_Oscilloscope += value;
                Average_Oscilloscope = Sum_Oscilloscope / count;
                ui->Average->display(Average_Oscilloscope);

                if (value > Max_Oscilloscope)
                {
                    Max_Oscilloscope = value;
                }
                ui->Max->display(Max_Oscilloscope);

                if (value < Min_Oscilloscope)
                {
                    Min_Oscilloscope = value;
                }
                ui->Min->display(Min_Oscilloscope);
            }
        }
    }
    Oscilloscope_Data.clear();
}

void Widget::on_Clear_Oscilloscope_clicked()
{
    Oscilloscope_Data.clear();
}
