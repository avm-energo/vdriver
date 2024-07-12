#include <include/vdriver/tcpclient.h>

TCPClient::TCPClient(QObject *parent)
    : QTcpSocket{parent}
{}
