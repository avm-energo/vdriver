#include <include/vdriver/serviceparser.h>

ServiceParser::ServiceParser(QObject *parent)
    : QObject{parent}
{}

void ServiceParser::newDataReceivedFromSocket(QByteArray &ba)
{
    // the code of modbus TCP parser would be here
    emit writeToSocket(ba);
}
