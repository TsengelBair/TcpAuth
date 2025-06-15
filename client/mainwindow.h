#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTcpSocket>
#include <QPushButton>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

enum RequestType
{
    REGISTER_REQUEST = 0,
    LOGIN_REQUEST = 1
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void sendToServer();

private:
    RequestType getReqType(QPushButton* btn);
    QPair<QString, QString> getDataFromUi();
    QByteArray serializeData(const QString &login, const QString &password);
    QByteArray createPacket(const QByteArray &data, const RequestType reqType);
    uint8_t calcCrc(const QByteArray& data);

private:
    Ui::MainWindow *ui;
    QTcpSocket* m_socket;
};
#endif // MAINWINDOW_H
