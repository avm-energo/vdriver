#ifndef TCPCLIENT_H
#define TCPCLIENT_H

#include <QObject>
#include <QTcpSocket>

class TCPClient : public QTcpSocket
{
    Q_OBJECT
public:
    explicit TCPClient(QObject *parent = nullptr);
};

#endif // TCPCLIENT_H
