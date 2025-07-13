#include <QTcpSocket>
#include <QDataStream>

#include "server.h"
#include "../IAuthRequest.pb.h"
#include "IDbHandler.h"
#include "structs.h"

Server::Server(IDbHandler &db) : m_db(db)
{
    if (this->listen(QHostAddress::LocalHost, 5000)) {
        qDebug() << "Listen";
    }
    else {
        qDebug() << "Error: " << errorString();
    }
}

Server::~Server()
{
    for (auto socket : m_sockets) {
        socket->disconnectFromHost();
        socket->deleteLater();
    }

    this->close();
}

void Server::incomingConnection(qintptr socketDescriptor)
{
    QTcpSocket* socket = new QTcpSocket();
    socket->setSocketDescriptor(socketDescriptor);

    m_sockets.insert(socketDescriptor, socket);
    qDebug() << "Client connected: " << socketDescriptor;

    connect(socket, &QTcpSocket::readyRead, this, &Server::handleClientRequest);
    connect(socket, &QTcpSocket::disconnected, socket, &QTcpSocket::deleteLater);
}

void Server::handleClientRequest()
{
    QTcpSocket* socket = qobject_cast<QTcpSocket*>(sender());
    if (!socket) {
        qDebug() << "Ошибка каста в сокет";
        return;
    }

    if (socket->bytesAvailable() < 6) {
        qDebug() << "Пришло меньше 6 байт";
        return;
    }

    QByteArray packet = socket->readAll();
    bool isValid = validatePacket(packet);
    if (!isValid) {
        qDebug() << "Validation error";
        return;
    }

    /// десериализовать
    IAuthRequest authReq;
    if (!authReq.ParseFromString(packet.mid(6).toStdString())) {
        qDebug() << "Error while deserialize auth request";
        return;
    }

    QString login = QString::fromStdString(authReq.login());
    QString password = QString::fromStdString(authReq.password());

    /// Извлечь тип запроса
    RequestType reqType = static_cast<RequestType>(packet.at(5));
    if (reqType == REGISTER_REQUEST) {
        RegisterStatus res = m_db.registerUser(login, password);
        qDebug() << res;
    }
    else if (reqType == LOGIN_REQUEST) {
        AuthStatus res = m_db.authUser(login, password);
        qDebug() << res;
    }
}

bool Server::validatePacket(const QByteArray &packet)
{
    QByteArray header = packet.left(6 - 1);
    QDataStream ds(&header, QIODevice::ReadOnly);

    quint32 dataSizeFromPacket;
    uint8_t crcFromPacket;

    ds >> dataSizeFromPacket >> crcFromPacket;

    QByteArray packetData = packet.mid(6);
    if (packetData.size() != dataSizeFromPacket) {
        qDebug() << "Length mismatch";
        return false;
    }

    uint8_t crc = calcCrc(packetData);
    if (crc != crcFromPacket) {
        qDebug() << "Crc mismatch";
        return false;
    }

    return true;
}

uint8_t Server::calcCrc(const QByteArray &data)
{
    uint8_t crc = 0;
    for (const char c : data) {
        crc ^= static_cast<uint8_t>(c);
    }

    return crc;
}
