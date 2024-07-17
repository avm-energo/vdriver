#ifndef MODBUSTCPSERVER_H
#define MODBUSTCPSERVER_H

#include <QByteArray>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class ModbusTCPServer : public QObject
{
    Q_OBJECT
public:
    ModbusTCPServer(int port, QObject *parent = nullptr);

    bool start();

public slots:
    void writeData(const QByteArray &ba);

private:
    QTcpServer *m_server;
    QTcpSocket *m_clientConnection;
    int m_port;

signals:
    void newDataReady(QByteArray &ba);
    void writeNewData(const QByteArray &ba);

private slots:
    void clientTryingToConnect();
    void newDataReceived();
};

#endif // MODBUSTCPSERVER_H
