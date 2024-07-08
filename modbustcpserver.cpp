#include "modbustcpserver.h"

ModbusTCPServer::ModbusTCPServer(QObject *parent)
    : QObject(parent)
{
    m_server = new QTcpServer(this);
    if (!tcpServer->listen()) {
        QMessageBox::critical(this,
                              tr("Fortune Server"),
                              tr("Unable to start the server: %1.").arg(tcpServer->errorString()));
        close();
        return;
    }
}
