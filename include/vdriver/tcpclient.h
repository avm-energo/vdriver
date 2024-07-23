#pragma once

#include <QObject>
#include <QTcpSocket>

class TCPClient : public QObject
{
    Q_OBJECT
public:
    explicit TCPClient(QObject *parent = nullptr);
    ~TCPClient();

    bool init(const QString &ip, int port);
    void setName(const QString &name);

    bool status;

public slots:
    void writeData(const QByteArray &ba);

signals:
    void newDataReady(QByteArray &ba);

private slots:
    void errorOccured(QAbstractSocket::SocketError error);
    void newDataReceived();

private:
    QTcpSocket *m_socket;
    QString m_name;
    QDataStream m_in;
};
