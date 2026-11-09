#include "mainwindow.h"

#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QPushButton>
#include <QPlainTextEdit>
#include <QLabel>
#include <QMessageBox>
#include <QDateTime>

using namespace Qt::StringLiterals;

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    setWindowTitle("MQTT 测试");
    resize(680, 520);

    m_client = new QMqttClient(this);
    m_client->setHostname(u"broker.emqx.io"_s);   // 公共测试 broker
    m_client->setPort(1883);

    // ---------- 界面 ----------
    auto *central = new QWidget(this);
    auto *root = new QVBoxLayout(central);

    auto *form = new QFormLayout;
    m_hostEdit = new QLineEdit(u"broker.emqx.io"_s, central);
    m_portSpin = new QSpinBox(central);
    m_portSpin->setRange(1, 65535);
    m_portSpin->setValue(1883);
    m_userEdit = new QLineEdit(central);
    m_userEdit->setPlaceholderText(u"可选"_s);
    m_passEdit = new QLineEdit(central);
    m_passEdit->setEchoMode(QLineEdit::Password);
    m_passEdit->setPlaceholderText(u"可选"_s);
    form->addRow(u"Broker 地址:"_s, m_hostEdit);
    form->addRow(u"端口:"_s, m_portSpin);
    form->addRow(u"用户名:"_s, m_userEdit);
    form->addRow(u"密码:"_s, m_passEdit);
    root->addLayout(form);

    m_connectBtn = new QPushButton(u"连接"_s, central);
    root->addWidget(m_connectBtn);

    auto *subBar = new QHBoxLayout;
    m_subTopicEdit = new QLineEdit(u"test/topic/qt"_s, central);
    m_subBtn = new QPushButton(u"订阅"_s, central);
    subBar->addWidget(new QLabel(u"订阅主题:"_s, central));
    subBar->addWidget(m_subTopicEdit, 1);
    subBar->addWidget(m_subBtn);
    root->addLayout(subBar);

    auto *pubBar = new QHBoxLayout;
    m_pubTopicEdit = new QLineEdit(u"test/topic/qt"_s, central);
    m_pubMsgEdit = new QLineEdit(u"hello from Qt"_s, central);
    m_pubBtn = new QPushButton(u"发布"_s, central);
    pubBar->addWidget(new QLabel(u"发布:"_s, central));
    pubBar->addWidget(m_pubTopicEdit, 1);
    pubBar->addWidget(m_pubMsgEdit, 1);
    pubBar->addWidget(m_pubBtn);
    root->addLayout(pubBar);

    m_log = new QPlainTextEdit(central);
    m_log->setReadOnly(true);
    root->addWidget(m_log, 1);

    m_stateLabel = new QLabel(u"状态: 已断开"_s, central);
    root->addWidget(m_stateLabel);

    setCentralWidget(central);

    // ---------- 信号槽 ----------
    connect(m_client, &QMqttClient::stateChanged,
            this, &MainWindow::updateLogStateChange);

    connect(m_client, &QMqttClient::errorChanged, this,
            [this](QMqttClient::ClientError e) {
                if (e != QMqttClient::NoError)
                    log(u"错误 code=%1"_s.arg(int(e)));
            });

    connect(m_client, &QMqttClient::messageReceived, this,
            [this](const QByteArray &msg, const QMqttTopicName &topic) {
                log(u"[收到] %1 : %2"_s.arg(topic.name(), QString::fromUtf8(msg)));
            });

    connect(m_connectBtn, &QPushButton::clicked, this, &MainWindow::connectClicked);
    connect(m_subBtn,     &QPushButton::clicked, this, &MainWindow::subscribeClicked);
    connect(m_pubBtn,     &QPushButton::clicked, this, &MainWindow::publishClicked);
}

void MainWindow::connectClicked()
{
    if (m_client->state() == QMqttClient::Disconnected) {
        m_client->setHostname(m_hostEdit->text());
        m_client->setPort(static_cast<quint16>(m_portSpin->value()));
        m_client->setUsername(m_userEdit->text());
        m_client->setPassword(m_passEdit->text());
        m_client->connectToHost();
    } else {
        m_client->disconnectFromHost();
    }
}

void MainWindow::subscribeClicked()
{
    auto *sub = m_client->subscribe(m_subTopicEdit->text(), 0);
    if (!sub) {
        QMessageBox::warning(this, u"订阅失败"_s, u"请先连接到 broker"_s);
        return;
    }
    log(u"已订阅: %1"_s.arg(m_subTopicEdit->text()));
}

void MainWindow::publishClicked()
{
    qint32 id = m_client->publish(m_pubTopicEdit->text(),
                                  m_pubMsgEdit->text().toUtf8());
    if (id == -1) {
        QMessageBox::warning(this, u"发布失败"_s, u"请先连接到 broker"_s);
    } else {
        log(u"[发布] %1 : %2 (msgId=%3)"_s.arg(m_pubTopicEdit->text(),
                                               m_pubMsgEdit->text(),
                                               QString::number(id)));
    }
}

void MainWindow::updateLogStateChange(QMqttClient::ClientState s)
{
    QString text;
    switch (s) {
    case QMqttClient::Disconnected: text = "已断开"; break;
    case QMqttClient::Connecting:   text = "连接中"; break;
    case QMqttClient::Connected:    text = "已连接"; break;
    }
    m_stateLabel->setText(u"状态: %1"_s.arg(text));
    m_connectBtn->setText(s == QMqttClient::Connected ? u"断开连接"_s : u"连接"_s);
    log(u"状态变更: %1"_s.arg(text));
}

void MainWindow::log(const QString &s)
{
    m_log->appendPlainText(QDateTime::currentDateTime().toString(u"hh:mm:ss "_s) + s);
}
