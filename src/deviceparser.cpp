#include <include/vdriver/deviceparser.h>
#include <include/vdriver/ModCRC.h>

DeviceParser::DeviceParser(TCPClient *client, QObject *parent)
    : m_client(client), QObject{parent}
{

    m_RTUTimer = new QTimer;
    m_RTUTimer->setInterval(1000);
    connect(m_RTUTimer, &QTimer::timeout, this, &DeviceParser::RTUTimerTimeout);
}

void DeviceParser::setName(const QString &name)
{
    m_name = "[Parser][" + name + "]";
}

void DeviceParser::newDataReceivedFromTCP(QByteArray &ba)
{
   unsigned short qCRC;
   QByteArray   ba2;

   bin=ba.left(8);
    // the code of modbus TCP parser would be here
    if (m_client->status == false)
    {
        // generate error message to TCP
        ba2=bin;
        ba2.data()[7]= 0x80+ba2.data()[7];
        ba2.append(0x0A);
        ba2.data()[4]= 0;
        ba2.data()[5]= 3;
        emit writeToTCP(ba2);
        return;
    }


    qsizetype len = ba.size();
    if(len<7)
        return;


    ba2 = ba.right(len-7);
    qCRC=CalcCRC(ba2);

  //  ba2 = ba.right(len-6);
//    ba2=bin;
    ba2=bin+ba.right(len-6);
    ba2.append(*((unsigned char *)&qCRC));
    ba2.append(*((unsigned char *)&qCRC+1));


    m_RTUTimer->start();
    emit writeToRTU(ba2);
}

void DeviceParser::newDataReceivedFromRTU(QByteArray &ba)
{
    unsigned short qCRC,rCRC,ln;
    unsigned char c1,c2;
     QByteArray   ba2,ba3;

    m_RTUTimer->stop();
    // the code of modbus RTU parser would be here

    qsizetype len = ba.size();
    c1=ba.data()[len-2];
    c2=ba.data()[len-1];

    *((char *)&rCRC+1)=c1;
    *((char *)&rCRC)=c2;

    ba2 = ba.right(len-1);
    ba2 = ba2.left(len-2);
     qCRC=CalcCRC(ba2);

     if(qCRC!=rCRC)
     {
         ba2=bin.left(9);
         ba2.data()[7]= 0x80+ba2.data()[7];
         ba2.data()[8]=0x0A;
         ba2.data()[4]= 0;
         ba2.data()[5]= 3;
         emit writeToTCP(ba2);
         return;
     }

    ba3=bin.left(6);
    ln=len-2;
    ba3.data()[4]=*((char *)&ln+1);
    ba3.data()[5]= *((char *)&ln);
    ba3=ba3+ba.left(ln);

        emit writeToTCP(ba);
}

void DeviceParser::RTUTimerTimeout()
{
   // emit writeToTCP(ba);
}

unsigned short DeviceParser:: CalcCRC(QByteArray &ba )
{
qsizetype len = ba.size();
unsigned char Dat,i;
unsigned char CRChi=0xFF;
unsigned char  CRClo=0xFF;
unsigned char  Ind;

  for(i=0;i<len ;i++)
    {
    Dat=ba.data()[i];
    Ind = CRChi ^ Dat;
    Dat++;
    CRChi = CRClo ^ TabCRChi[Ind];
    CRClo = TabCRClo[Ind];
    }
  return((CRChi<<8)|CRClo);
}
