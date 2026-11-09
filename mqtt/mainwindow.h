#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMqttClient>

class QLineEdit;
class QSpinBox;
class QPushButton;
class QPlainTextEdit;
class QLabel;

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void connectClicked();
    void subscribeClicked();
    void publishClicked();
    void updateLogStateChange(QMqttClient::ClientState state);

private:
    void log(const QString &s);

    QMqttClient *m_client = nullptr;

    QLineEdit      *m_hostEdit = nullptr;
    QSpinBox       *m_portSpin = nullptr;
    QLineEdit      *m_userEdit = nullptr;
    QLineEdit      *m_passEdit = nullptr;
    QPushButton    *m_connectBtn = nullptr;

    QLineEdit      *m_subTopicEdit = nullptr;
    QPushButton    *m_subBtn = nullptr;

    QLineEdit      *m_pubTopicEdit = nullptr;
    QLineEdit      *m_pubMsgEdit = nullptr;
    QPushButton    *m_pubBtn = nullptr;

    QPlainTextEdit *m_log = nullptr;
    QLabel         *m_stateLabel = nullptr;
};

#endif // MAINWINDOW_H
