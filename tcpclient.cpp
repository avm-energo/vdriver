#include "tcpclient.h"

TCPClient::TCPClient(QObject *parent)
    : QTcpSocket{parent}
{}
