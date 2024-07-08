#ifndef MODBUSTCPSERVER_H
#define MODBUSTCPSERVER_H

#include <QObject>
#include <QTcpServer>

class ModbusTCPServer : public QObject
{
    Q_OBJECT
public:
    ModbusTCPServer(QObject *parent = nullptr);

private:
    QTcpServer *m_server;
};

#endif // MODBUSTCPSERVER_H
