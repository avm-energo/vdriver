#include <include/vdriver/device.h>
#include <include/vdriver/deviceparser.h>

Device::Device(QObject *parent)
    : QObject{parent}
{}

Device::~Device()
{
    foreach (auto server, m_serverList) {
        server->disconnect();
        delete server;
    }
    foreach (auto client, m_clientList) {
        client->disconnect();
        delete client;
    }
}

bool Device::init(Settings::DeviceStruct &device)
{
    int currentPort = device.startPort;
    int startRTUPort = 10000;
    foreach (auto port, device.ports)
    {
        if(port.use)
        {
            const QString name = device.ip + QString::number(currentPort);
            TCPServer *server = new TCPServer(currentPort);
            server->setName(name);
            TCPClient *client = new TCPClient;
            client->setName(name);
            DeviceParser *parser = new DeviceParser(client);
            parser->setName(name);
            connect(server, &TCPServer::newDataReady, parser, &DeviceParser::newDataReceivedFromTCP);
            connect(parser, &DeviceParser::writeToTCP, server, &TCPServer::writeData);
            server->start();
            m_serverList.append(server);
            connect(client, &TCPClient::newDataReady, parser, &DeviceParser::newDataReceivedFromRTU);
            connect(parser, &DeviceParser::writeToRTU, client, &TCPClient::writeData);
            client->init(device.ip, startRTUPort++);
            m_clientList.append(client);
        }
        ++currentPort;
    }
    return true;
}
