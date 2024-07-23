#include <include/vdriver/serviceparser.h>

ServiceParser::ServiceParser(QObject *parent)
    : QObject{parent}
{}

void ServiceParser::newDataReceivedFromSocket(QByteArray &ba)
{
    qsizetype len = ba.size();
    char f;
    unsigned short ar;
    char b1,b2;
    // the code of modbus TCP parser would be here

    QByteArray ba2 = ba.left(8);
    f=ba.data()[7];
    switch(f)
    {
        case 3:




        break;

    default:
        ba.data()[7]= 0x80+ba.data()[7];
        ba.data()[8]=0x0A;
        break;

    }

    b1=ba.data()[8];
    b2=ba.data()[9];
    *((char *)&ar+1)=b1;
    *((char *)&ar)=b2;


    f=ba.data()[7];
    ba.data()[7]= 0x80+ba.data()[7];
     f=ba.data()[7];
     ba.data()[8]=0x0A;

     ba = ba.left(9);

    emit writeToSocket(ba);
}
