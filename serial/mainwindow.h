#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QMainWindow>
#include <QList>
#include <QProcess>
#include <QPushButton>
#include <QHBoxLayout>
#include <QByteArray>
#include <QCloseEvent>
#include <iostream>
#include <cstdio>
#include <string>
#include <cstring>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sstream>
#include <QTimer>
#include <QTime>
#include <QFile>
#include <QFileInfo>
#include <QProgressDialog>
#include <QButtonGroup>
#include <QThread>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void sendData(const QByteArray &data);
    QString bytesToHexString(const QByteArray &data);
    QByteArray hexStringToBytes(const QString &hexStr);
    void flushBuffer();

private slots:
    void readSerialData();
    void on_pushButton_clicked();
    void on_pushButton_6_clicked();
    void on_pushButton_8_clicked();
    void on_pushButton_9_clicked();
    void on_pushButton_10_clicked();
    void on_pushButton_11_clicked();
    void on_pushButton_7_clicked();
    void on_pushButton_sendFile_clicked();
    void onModeChanged(int id);

private:
    Ui::MainWindow *ui;
    QList<QString> list;
    QSerialPort *serialPort;
    QButtonGroup *modeGroup;

    time_t m_lasttime;
    QByteArray m_recvData;
    QTimer m_flushTimer;

    bool m_hexMode;
    bool applySerialConfig();

protected:
    void closeEvent(QCloseEvent *event) override;
};
#endif // MAINWINDOW_H
