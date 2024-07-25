#include <include/vdriver/settings.h>
#include <include/vdriver/tcpclient.h>

TCPClient::TCPClient(QObject *parent)
    : QObject{parent}
{
    m_socket = new QTcpSocket(this);
    m_in.setDevice(m_socket);
}

TCPClient::~TCPClient()
{
    m_socket->disconnect();
    delete m_socket;
}

bool TCPClient::init(const QString &ip, int port)
{
    m_reconnectTimer = new QTimer;
    m_reconnectTimer->setSingleShot(true);
    m_reconnectTimer->setInterval(Settings::reconnectPeriod()
                                  * 1000); // reconnectPeriod is set in seconds
    connect(m_reconnectTimer, &QTimer::timeout, this, &TCPClient::itsTimeToReconnect);
    m_ip = ip;
    m_port = port;
    connect(m_socket, &QAbstractSocket::readyRead, this, &TCPClient::newDataReceived);
    connect(m_socket, &QAbstractSocket::errorOccurred, this, &TCPClient::errorOccured);
    connect(m_socket, &QAbstractSocket::connected, this, &TCPClient::socketConnected);
    connect(m_socket, &QAbstractSocket::disconnected, this, &TCPClient::socketDisconnected);
    setName(ip + ":" + QString::number(port));
    qDebug() << "Connecting to host: " << ip << ":" << port;
    m_socket->connectToHost(ip, port);
    return true;
}

void TCPClient::setName(const QString &name)
{
    m_name = "[RTU][" + name + "] ";
}

void TCPClient::disconnectSocket() {}

void TCPClient::writeData(const QByteArray &ba)
{
    qint64 writtenDataSize = m_socket->write(ba);
    if (writtenDataSize != ba.size()) {
        qWarning() << m_name << "Write data size is not equal to data size actually written";
    }
}

void TCPClient::errorOccured(QAbstractSocket::SocketError error)
{
    switch (error) {
    case QAbstractSocket::RemoteHostClosedError:
        break;
    case QAbstractSocket::HostNotFoundError:
        qCritical() << m_name << "The host was not found";
        break;
    case QAbstractSocket::ConnectionRefusedError:
        qCritical() << m_name << "The connection was refused by the peer";
        break;
    default:
        qCritical() << m_name << "The following error occurred: " << m_socket->errorString();
    }
    m_reconnectTimer->start();
}

void TCPClient::newDataReceived()
{
    if (m_socket == nullptr) {
        qCritical() << m_name << "Socket is null";
        return;
    }
//    m_in.startTransaction();
    QByteArray ba = m_socket->readAll();
//    m_in >> ba;
//    if (!m_in.commitTransaction())
//        return;
    emit newDataReady(ba);
}

void TCPClient::socketConnected()
{
    qDebug() << "Host connected: " << m_name ;
    status = true;
}

void TCPClient::socketDisconnected()
{
    status = false;
    qDebug() << "Host disconnected: " << m_name << ", retrying in 15 sec";
    m_reconnectTimer->start();
}

void TCPClient::itsTimeToReconnect()
{
    qDebug() << "Reconnecting to host: " << m_name;
    m_socket->connectToHost(m_ip, m_port);
}
