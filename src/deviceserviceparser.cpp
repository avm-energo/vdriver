#include <include/vdriver/deviceserviceparser.h>

DeviceServiceParser::DeviceServiceParser(TCPClient *client,
                                         Settings::DeviceStruct devSettings,
                                         QObject *parent)
    : m_client(client), m_deviceSettings(devSettings), QObject{parent} {
  m_ConfTimer = new QTimer;
  m_ConfTimer->setInterval(1000);
  connect(m_ConfTimer, &QTimer::timeout, this,
          &DeviceServiceParser::ConfTimerTimeout);
}

void DeviceServiceParser::start(void) {
  QByteArray ba2;
  ba2.append('0');
  ba2.append('1');

  stepinitport = 0;
  initcurport = 0;
  emit writeToService(ba2);
}

void DeviceServiceParser::newDataReceived(QByteArray &ba) {
  QByteArray ba2, ba3, ba4;
  char np;
  int i;
  qsizetype num;
  bool fl;

  switch (stepinitport) {
  case 0:
    if (initcurport != 0) {
      if (ba.data()[0] == 'O' && ba.data()[1] == 'K') {
      } else {
        qWarning() << "  IP  " << m_deviceSettings.ip << " Com Port"
                   << initcurport << " error initialization  ";
        initcurport = 0;
        return;
      }
    }
    initcurport++;
    for (i = initcurport; i < 6; i++) {
      if (m_deviceSettings.ports[i - 1].use) {
        if (checkintport(i)) {
          stepinitport++;
          initcurport = i;
          getportstatus(i);

          return;
        } else
          noerr = false;
      } else
        continue;
    }
    if (noerr)
      isinitDev = true;
    qInfo() << "  IP  " << m_deviceSettings.ip << " configuratin  OK  ";
    break;

  case 1:

    ba2.setNum(m_deviceSettings.ports[initcurport - 1].speed);
    ba2.append(',');
    ba2.append('8');
    ba2.append(',');
    ba2.append(Cpar[m_deviceSettings.ports[initcurport - 1].parity]);
    ba2.append(',');
    ba3.setNum(m_deviceSettings.ports[initcurport - 1].stopbit);
    ba2 = ba2 + ba3;

    ba.chop(1);

    qInfo() << "  IP  " << m_deviceSettings.ip << " Com Port" << initcurport
            << " status  " << ba;

    if (ba == ba2) {
      initcurport++;
      for (i = initcurport; i < 6; i++) {
        if (m_deviceSettings.ports[i - 1].use) {
          if (checkintport(initcurport)) {
            initcurport = i;
            getportstatus(initcurport);
            return;
          } else
            noerr = false;
        } else
          continue;
      }
      if (noerr) {
        isinitDev = true;
        qInfo() << "  IP  " << m_deviceSettings.ip << " configuratin  OK  ";
      }
      break;
    } else {
      qInfo() << "  IP  " << m_deviceSettings.ip << " Com Port" << initcurport
              << " status in conf file  " << ba2;
      ba3.clear();
      num = ba.indexOf(",");
      ba3 = ba.sliced(num + 1);
      ba4.clear();
      ba4 = ba2.sliced(num + 1);

      if (ba3 != ba4)
        isdataformat = true;
      ba3.clear();
      ba3.setNum(m_deviceSettings.ports[initcurport - 1].speed);
      ba4.clear();
      ba4 = ba.first(num);

      if (ba3 != ba4) {
        if (isdataformat)
          stepinitport = 3;
        else {
          stepinitport = 0;
        }
        setbaud(initcurport);
      } else {
        setdataformat(initcurport);
        stepinitport = 0;
      }
    }
    break;

  case 3:
    if (ba.data()[0] == 'O' && ba.data()[1] == 'K') {
    } else {
      qWarning() << "  IP  " << m_deviceSettings.ip << " Com Port"
                 << initcurport << " error initialization  ";
      noerr = false;
      initcurport = 0;
      return;
    }

    stepinitport = 0;
    setdataformat(initcurport);

    break;
  };
}

void DeviceServiceParser::getportstatus(int &np) {
  QByteArray ba2, ba3;

  ba2.setNum(16);
  ba3.setNum(np);
  ba2 = ba2 + ba3;

  emit writeToService(ba2);
}

bool DeviceServiceParser::checkintport(int &np) {
  uint baud, parity, stopbit;
  bool noerr = true;

  baud = m_deviceSettings.ports[np - 1].speed;
  switch (baud) {
  case 1200:
  case 2400:
  case 4800:
  case 9600:
  case 19200:
  case 38400:
  case 57600:
  case 115200:
    break;
  default:
    noerr = false;
    qWarning() << "  IP  " << m_deviceSettings.ip << " Com Port" << np
               << " error baud in conf file  " << baud;
  }

  parity = m_deviceSettings.ports[np - 1].parity;
  if (parity > 2) {
    noerr = false;
    qWarning() << "  IP  " << m_deviceSettings.ip << " Com Port" << np
               << " error parity in conf file  ";
  }

  stopbit = m_deviceSettings.ports[np - 1].stopbit;
  if (stopbit != 1)
    if (!((stopbit == 2) && (parity == 0))) {
      noerr = false;
      qWarning() << "  IP  " << m_deviceSettings.ip << " Com Port" << np
                 << " error stop bity in conf file  " << stopbit;
    }

  return noerr;
}

void DeviceServiceParser::setbaud(int &np) {
  QByteArray ba2 = "06", ba3;

  //  ba2.setNum(0);
  //  ba2.append('6');
  ba3.setNum(np);
  ba2 = ba2 + ba3;

  ba3.setNum(m_deviceSettings.ports[np - 1].speed);
  ba2 = ba2 + ba3;

  qInfo() << "  IP  " << m_deviceSettings.ip << " Com Port" << initcurport
          << " set baud  " << ba3;

  emit writeToService(ba2);
}

void DeviceServiceParser::setdataformat(int &np) {
  QByteArray ba2 = "07", ba3;

  ba3.setNum(np);
  ba2 = ba2 + ba3;

  ba2.append('8');
  ba2.append(Cpar[m_deviceSettings.ports[np - 1].parity]);

  ba3.setNum(m_deviceSettings.ports[np - 1].stopbit);
  ba2 = ba2 + ba3;

  ba3.clear();
  ba3 = ba2.sliced(3, 3);

  qInfo() << "  IP  " << m_deviceSettings.ip << " Com Port" << initcurport
          << " set data format  " << ba3;

  emit writeToService(ba2);
}

void DeviceServiceParser::ConfTimerTimeout() {

  qWarning() << "  IP  " << m_deviceSettings.ip
             << " error time-out configuratin ";
}
