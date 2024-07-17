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
        return false;
    }
    connect(m_server, &QTcpServer::newConnection, this, &ModbusTCPServer::clientTryingToConnect);
    return true;
}

void ModbusTCPServer::writeData(const QByteArray &ba)
{
    qint64 writtenDataSize = m_clientConnection->write(ba);
    if (writtenDataSize != ba.size()) {
        qWarning() << "Write data size is not equal to data size actually written";
    }
}

void ModbusTCPServer::clientTryingToConnect()
{
    m_clientConnection = m_server->nextPendingConnection();
    connect(m_clientConnection,
            &QAbstractSocket::disconnected,
            m_clientConnection,
            &QObject::deleteLater);
    connect(m_clientConnection, &QIODevice::readyRead, this, &ModbusTCPServer::newDataReceived);
}

void ModbusTCPServer::newDataReceived()
{
    if (m_clientConnection == nullptr) {
        qFatal() << "Tcp socket is null";
        return;
    }
    QByteArray ba = m_clientConnection->readAll();
    emit newDataReady(ba);
}
