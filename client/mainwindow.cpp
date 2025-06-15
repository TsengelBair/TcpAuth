#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include "../IAuthRequest.pb.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    m_socket = new QTcpSocket(this);
    m_socket->connectToHost("127.0.0.1", 5000);

    connect(ui->registrBtn, &QPushButton::clicked, this, &MainWindow::sendToServer, Qt::UniqueConnection);
    connect(ui->loginBtn, &QPushButton::clicked, this, &MainWindow::sendToServer, Qt::UniqueConnection);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::sendToServer()
{
    QPushButton* btn = qobject_cast<QPushButton*> (sender());
    RequestType reqType = getReqType(btn);

    QPair<QString, QString> loginAndPassword = getDataFromUi();
    if (loginAndPassword.first.isEmpty() || loginAndPassword.second.isEmpty()) return;

    QByteArray serializedData = serializeData(loginAndPassword.first, loginAndPassword.second);
    if (serializedData.isEmpty()) return;

    QByteArray packet = createPacket(serializedData, reqType);

    m_socket->write(packet);
}

RequestType MainWindow::getReqType(QPushButton *btn)
{
    RequestType reqType;

    if (btn->text() == "Register") reqType = RequestType::REGISTER_REQUEST;
    else reqType = RequestType::LOGIN_REQUEST;

    return reqType;
}

QPair<QString, QString> MainWindow::getDataFromUi()
{
    QString login = ui->loginLE->text();
    QString password = ui->passwordLE->text();

    if (login.isEmpty() || password.isEmpty()) {
        ui->statusbar->showMessage("Заполните все поля");
        return QPair<QString, QString>();
    }

    return qMakePair(login, password);
}

QByteArray MainWindow::serializeData(const QString &login, const QString &password)
{
    IAuthRequest request;

    request.set_login(login.toStdString());
    request.set_password(password.toStdString());

    std::string serializedData;
    if (!request.SerializeToString(&serializedData)) {
        qDebug() << "Ошибка при сериализации";
        return QByteArray();
    }

    QByteArray data(serializedData.c_str(), serializedData.size());;
    return data;
}

QByteArray MainWindow::createPacket(const QByteArray &data, const RequestType reqType)
{
    QByteArray packet;

    quint32 packetSize = data.size();

    QDataStream out(&packet, QIODevice::WriteOnly);
    out << quint32(packetSize);

    uint8_t crc = calcCrc(data);
    packet.append(static_cast<char>(crc));
    packet.append(static_cast<char>(reqType));
    packet.append(data);

    return packet;
}

uint8_t MainWindow::calcCrc(const QByteArray &data)
{
    uint8_t crc = 0;
    for (const char c : data) {
        crc ^= static_cast<uint8_t>(c);
    }

    return crc;
}



