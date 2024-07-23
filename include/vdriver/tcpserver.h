#pragma once

#include <QByteArray>
#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>

class TCPServer : public QObject
{
    Q_OBJECT
public:
    TCPServer(int port, QObject *parent = nullptr);

    bool start(QHostAddress address = QHostAddress::AnyIPv4);
    void setName(const QString &name);

public slots:
    void writeData(const QByteArray &ba);

private:
    QTcpServer *m_server;
    QTcpSocket *m_clientConnection;
    int m_port;
    QString m_name;

signals:
    void newDataReady(QByteArray &ba);

private slots:
    void clientTryingToConnect();
    void newDataReceived();
};
