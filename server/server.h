#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

enum RequestType
{
    REGISTER_REQUEST = 0,
    LOGIN_REQUEST = 1
};

class IDbHandler;

class Server : public QTcpServer
{
public:
    explicit Server(IDbHandler &db);
    ~Server();

private slots:
    void incomingConnection(qintptr socketDescriptor) override;
    void handleClientRequest();
    bool validatePacket(const QByteArray &packet);
    uint8_t calcCrc(const QByteArray &data);

private:
    QMap<qintptr, QTcpSocket*> m_sockets;
    IDbHandler &m_db;
};

#endif // SERVER_H
