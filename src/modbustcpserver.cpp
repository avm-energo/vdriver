#include <QtDebug>
#include <include/vdriver/modbustcpserver.h>

ModbusTCPServer::ModbusTCPServer(int port, QObject *parent)
    : m_port(port)
    , QObject(parent)
{
    m_server = new QTcpServer(this);
}

bool ModbusTCPServer::start()
{
    if (!m_server->listen(QHostAddress::Any, m_port)) {
        qDebug() << "Unable to start the server: " << m_server->errorString();
        // close();
        return false;
    }
    return true;
}
