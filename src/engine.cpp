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
    m_server = new ModbusTCPServer(settings.servicePort);
    connect(m_server,
            &ModbusTCPServer::newDataReady,
            parser,
            &ServiceParser::newDataReceivedFromSocket);
    connect(parser, &ServiceParser::writeToSocket, m_server, &ModbusTCPServer::writeData);

    // создаём клиенты
    settings.deviceList();
}
