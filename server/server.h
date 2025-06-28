#ifndef SERVER_H
#define SERVER_H

#include <QTcpServer>

class Server : public QTcpServer
{
public:
    Server();
    ~Server();

private slots:
    void incomingConnection(qintptr socketDescriptor) override;
    void handleClientRequest();
    bool validatePacket(const QByteArray &packet);
    uint8_t calcCrc(const QByteArray &data);

private:
    QMap<qintptr, QTcpSocket*> m_sockets;
};

#endif // SERVER_H
