#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QDebug>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QButtonGroup>
#include <QThread>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    m_lasttime = 0;
    m_hexMode = false;

    ui->setupUi(this);

    // 创建串口对象
    serialPort = new QSerialPort(this);

    // 搜索所有可用串口
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        QSerialPort serial;
        serial.setPort(info);
        ui->comboBox->addItem(serial.portName());
        if (serial.open(QIODevice::ReadWrite)){
            qDebug() << "==>" << serial.portName();
        }
    }

    ui->comboBox_2->setCurrentText("9600");

    // 设置定时器（500ms无新数据则刷新）
    m_flushTimer.setInterval(500);
    connect(&m_flushTimer, &QTimer::timeout, this, &MainWindow::flushBuffer);

    // 设置模式选择按钮组
    modeGroup = new QButtonGroup(this);
    modeGroup->addButton(ui->radioButton_char, 0);
    modeGroup->addButton(ui->radioButton_hex, 1);
    connect(modeGroup, QOverload<int>::of(&QButtonGroup::buttonClicked),
            this, &MainWindow::onModeChanged);

    // 默认选择字符模式
    ui->radioButton_char->setChecked(true);
    // 默认选择：数据位 8、停止位 1、校验位 None
    ui->comboBox_dataBits->setCurrentText("8");
    ui->comboBox_stopBits->setCurrentText("1");
    ui->comboBox_parity->setCurrentText("None");
    ui->comboBox_flowControl->setCurrentText("None");
}

MainWindow::~MainWindow()
{
    serialPort->close();
    delete ui;
}

// 把界面上所有串口配置项应用到 serialPort
// 必须在 open() 之前调用；返回 false 表示某项设置失败
bool MainWindow::applySerialConfig()
{
    // 串口号与波特率
    serialPort->setPortName(ui->comboBox->currentText());
    serialPort->setBaudRate(ui->comboBox_2->currentText().toInt());

    // 数据位
    bool ok = false;
    int dbits = ui->comboBox_dataBits->currentText().toInt(&ok);
    if (ok) {
        switch (dbits) {
        case 5: serialPort->setDataBits(QSerialPort::Data5); break;
        case 6: serialPort->setDataBits(QSerialPort::Data6); break;
        case 7: serialPort->setDataBits(QSerialPort::Data7); break;
        case 8: serialPort->setDataBits(QSerialPort::Data8); break;
        default: serialPort->setDataBits(QSerialPort::Data8); break;
        }
    }

    // 停止位
    const QString sbits = ui->comboBox_stopBits->currentText();
    if (sbits == "1")
        serialPort->setStopBits(QSerialPort::OneStop);
    else if (sbits == "1.5")
        serialPort->setStopBits(QSerialPort::OneAndHalfStop);
    else if (sbits == "2")
        serialPort->setStopBits(QSerialPort::TwoStop);
    else
        serialPort->setStopBits(QSerialPort::OneStop);

    // 校验位
    const QString parity = ui->comboBox_parity->currentText();
    if (parity == "None")       serialPort->setParity(QSerialPort::NoParity);
    else if (parity == "Even")  serialPort->setParity(QSerialPort::EvenParity);
    else if (parity == "Odd")   serialPort->setParity(QSerialPort::OddParity);
    else if (parity == "Space") serialPort->setParity(QSerialPort::SpaceParity);
    else if (parity == "Mark")  serialPort->setParity(QSerialPort::MarkParity);
    else                        serialPort->setParity(QSerialPort::NoParity);

    // 流控制
    const QString flow = ui->comboBox_flowControl->currentText();
    if (flow == "None")
        serialPort->setFlowControl(QSerialPort::NoFlowControl);
    else if (flow == "Hardware")
        serialPort->setFlowControl(QSerialPort::HardwareControl);
    else if (flow == "Software")
        serialPort->setFlowControl(QSerialPort::SoftwareControl);
    else
        serialPort->setFlowControl(QSerialPort::NoFlowControl);

    // 校验每一项是否设置成功（部分平台可能不支持某些组合）
    if (serialPort->error() != QSerialPort::NoError) {
        qWarning() << "串口配置警告:" << serialPort->errorString();
        return false;
    }
    return true;
}

void MainWindow::onModeChanged(int id)
{
    m_hexMode = (id == 1);

    if (m_hexMode) {
        ui->textEdit->setPlaceholderText("请输入十六进制数据，如: 48 65 6C 6C 6F");
    } else {
        ui->textEdit->setPlaceholderText("请输入文本数据");
    }

    // 立即刷新当前缓冲区内容
    if (!m_recvData.isEmpty()) {
        flushBuffer();
    }
}

void MainWindow::on_pushButton_clicked()
{
    QString fileName = QFileDialog::getOpenFileName(this, tr("Open File"),
                                                    "/dev",
                                                    tr("All Files (*)"));
    if (!fileName.isEmpty()) {
        qDebug() << "Selected file: " << fileName;
    }
    ui->lineEdit->setText(fileName);
}

void MainWindow::on_pushButton_6_clicked()
{
    if(ui->lineEdit->text() != "") list.append(ui->lineEdit->text());
    if(ui->lineEdit_2->text() != "") list.append(ui->lineEdit_2->text());
    if(ui->lineEdit_3->text() != "") list.append(ui->lineEdit_3->text());
    if(ui->lineEdit_4->text() != "") list.append(ui->lineEdit_4->text());

    foreach(QString value, list) {
        ui->comboBox->addItem(value);
    }

    QMessageBox messageBox;
    messageBox.setWindowTitle("提示框");
    messageBox.setText("\n\n\n\t\t串并口添加成功!\t\t\n\n\n");
    messageBox.exec();
}

void MainWindow::on_pushButton_8_clicked()
{
    if (ui->pushButton_8->text() == tr("打开"))
    {
        // 统一应用所有串口配置（含新加的数据位/停止位/校验位/流控）
        if (!applySerialConfig()) {
            QMessageBox::warning(this, "错误",
                "部分串口参数配置失败，请检查参数组合是否被当前平台支持。");
            return;
        }

        // 打开串口
        if (serialPort->open(QIODevice::ReadWrite)) {
            qDebug() << "串口打开成功:"
                     << serialPort->portName()
                     << "波特率:" << serialPort->baudRate()
                     << "数据位:" << serialPort->dataBits()
                     << "停止位:" << serialPort->stopBits()
                     << "校验:"   << serialPort->parity()
                     << "流控:"   << serialPort->flowControl();
            m_recvData.clear();
            connect(serialPort, &QSerialPort::readyRead,
                    this, &MainWindow::readSerialData);
        } else {
            qDebug() << "串口打开失败:" << serialPort->errorString();
            QMessageBox::warning(this, "错误",
                "无法打开串口: " + serialPort->portName()
                + "\n原因: " + serialPort->errorString());
            return;
        }

        ui->pushButton_8->setText("关闭");
        ui->comboBox->setEnabled(false);
        ui->comboBox_2->setEnabled(false);
        ui->comboBox_dataBits->setEnabled(false);
        ui->comboBox_stopBits->setEnabled(false);
        ui->comboBox_parity->setEnabled(false);
        ui->comboBox_flowControl->setEnabled(false);
    }
    else
    {
        serialPort->close();
        qDebug() << "串口关闭成功";
        ui->pushButton_8->setText("打开");
        ui->comboBox->setEnabled(true);
        ui->comboBox_2->setEnabled(true);
        ui->comboBox_dataBits->setEnabled(true);
        ui->comboBox_stopBits->setEnabled(true);
        ui->comboBox_parity->setEnabled(true);
        ui->comboBox_flowControl->setEnabled(true);
    }
}


// 发送数据函数
void MainWindow::sendData(const QByteArray &data)
{
    if (!serialPort->isOpen()) {
        QMessageBox::warning(this, "警告", "请先打开串口！");
        return;
    }

    qint64 bytesWritten = serialPort->write(data);
    if (bytesWritten == -1) {
        QMessageBox::warning(this, "错误", "发送数据失败");
    }
    else if (bytesWritten != data.size()) {
        QMessageBox::warning(this, "警告", "数据未完全发送");
    }
}

// 字节数组转十六进制字符串
QString MainWindow::bytesToHexString(const QByteArray &data)
{
    return QString(data.toHex(' ').toUpper());
}

// 十六进制字符串转字节数组
QByteArray MainWindow::hexStringToBytes(const QString &hexStr)
{
    QByteArray byteArray;
    QString trimmed = hexStr.trimmed();
    QStringList hexValues = trimmed.split(' ');

    for (const QString &hexValue : hexValues) {
        if (hexValue.isEmpty()) continue;

        bool ok;
        char byte = static_cast<char>(hexValue.toInt(&ok, 16));
        if (ok) {
            byteArray.append(byte);
        }
        else {
            QMessageBox::warning(this, "错误", "无效的十六进制值: " + hexValue);
            return QByteArray();
        }
    }

    return byteArray;
}

void MainWindow::on_pushButton_9_clicked()
{
    QString sendText = ui->textEdit->toPlainText();

    if (m_hexMode) {
        // 十六进制模式
        QByteArray data = hexStringToBytes(sendText);
        if (!data.isEmpty()) {
            qDebug() << "发送十六进制数据:" << bytesToHexString(data);
            sendData(data);
        }
    }
    else {
        // 字符模式
        QByteArray data = sendText.toUtf8();
        qDebug() << "发送字符数据:" << sendText;
        sendData(data);
    }
}

void MainWindow::on_pushButton_10_clicked()
{
    ui->textEdit->clear();
}

void MainWindow::on_pushButton_11_clicked()
{
    ui->listWidget->clear();
}

void MainWindow::on_pushButton_7_clicked()
{
    ui->lineEdit->setText("");
    ui->lineEdit_2->setText("");
    ui->lineEdit_3->setText("");
    ui->lineEdit_4->setText("");
}

// 接收数据
void MainWindow::readSerialData() {
    m_recvData += serialPort->readAll();
    m_flushTimer.start();
}

// 刷新缓冲区并显示数据
void MainWindow::flushBuffer() {
    if (!m_recvData.isEmpty()) {
        QString displayData;

        if (m_hexMode) {
            // 十六进制模式
            displayData = bytesToHexString(m_recvData);
        }
        else {
            // 字符模式
            displayData = QString::fromUtf8(m_recvData);
        }

        ui->listWidget->addItem("[" + QTime::currentTime().toString() + "] " + displayData);
        ui->listWidget->scrollToBottom();
        m_recvData.clear();
    }

    m_flushTimer.stop();
}

// 发送文件
void MainWindow::on_pushButton_sendFile_clicked()
{
    if (!serialPort->isOpen()) {
        QMessageBox::warning(this, "警告", "请先打开串口！");
        return;
    }

    // 直接弹出文件选择对话框
    QString fileName = QFileDialog::getOpenFileName(this, "选择要发送的文件", "", "所有文件 (*)");
    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "错误", "无法打开文件: " + fileName);
        return;
    }

    QFileInfo fileInfo(fileName);
    qDebug() << "准备发送文件:" << fileName << "大小:" << fileInfo.size() << "字节";

    // 创建进度对话框
    QProgressDialog progress("发送文件中...", "取消", 0, file.size(), this);
    progress.setWindowTitle("发送文件");
    progress.setWindowModality(Qt::WindowModal);
    progress.show();

    // 分块读取和发送文件
    const int chunkSize = 1024;
    qint64 totalSent = 0;
    QByteArray data;

    while (!file.atEnd()) {
        if (progress.wasCanceled()) {
            break;
        }

        data = file.read(chunkSize);

        qint64 bytesWritten = serialPort->write(data);
        if (bytesWritten == -1) {
            QMessageBox::warning(this, "错误", "发送文件失败");
            break;
        }

        if (!serialPort->waitForBytesWritten(5000)) {
            QMessageBox::warning(this, "错误", "写入超时");
            break;
        }

        totalSent += bytesWritten;
        progress.setValue(totalSent);
        QApplication::processEvents();

        // 添加小延迟，避免过快发送导致接收方处理不过来
        QThread::msleep(10);
    }

    file.close();

    if (totalSent == fileInfo.size()) {
        QMessageBox::information(this, "完成", "文件发送完成！\n发送字节数: " + QString::number(totalSent));
    } else {
        QMessageBox::information(this, "中断", "文件发送已中断\n已发送字节数: " + QString::number(totalSent));
    }
}

// 窗口关闭事件
void MainWindow::closeEvent(QCloseEvent *event) {
    flushBuffer();
    event->accept();
}
