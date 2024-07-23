#include <include/vdriver/device.h>
#include <include/vdriver/engine.h>
#include <include/vdriver/serviceparser.h>

Engine::Engine(QObject *parent)
    : QObject{parent}
{}

Engine::~Engine()
{
    delete m_server;
}

void Engine::init(Settings &settings)
{
    ServiceParser *parser = new ServiceParser;
    // создаём сервер
    m_server = new TCPServer(settings.servicePort);
    connect(m_server, &TCPServer::newDataReady, parser, &ServiceParser::newDataReceivedFromSocket);
    connect(parser, &ServiceParser::writeToSocket, m_server, &TCPServer::writeData);
    m_server->start();

    // создаём
    foreach (auto deviceDescr, settings.deviceList()) {
        Device *dev = new Device();
         if (!dev->init(deviceDescr)) {
            qCritical() << "Cannot initialize device " << deviceDescr.ip << ":"
                        << deviceDescr.startPort;
        }
    }
}
