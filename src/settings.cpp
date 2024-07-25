#include <QtDebug>
#include "qforeach.h"
#include <gen/files.h>
#include <gen/logger.h>
#include <include/vdriver/defaults.h>
#include <include/vdriver/settings.h>

void Settings::init(QString &logFileName)
{
#ifdef LOCALDEBUG
    m_logDirectory = QDir::homePath() + "/.local/share/vdriver/";
    m_configDirectory = QDir::homePath() + "/.config/vdriver/";
#else
    m_logDirectory = "/var/log/";
    m_configDirectory = "/usr/local/etc/vdriver/";
#endif
    QDir dir;
    dir.mkpath(m_logDirectory);
    dir.mkpath(m_configDirectory + "conf.d");
    m_settings = new QSettings(m_configDirectory + Defaults::settingsFileName,
                               QSettings::NativeFormat);
    logFileName = m_settings->value("Logs/logfile", m_logDirectory + Defaults::logFileName)
                      .toString();
}

void Settings::readSettings()
{
    int logcounter = m_settings->value("Test/counter", "1").toInt();
    m_settings->setValue("Test/counter", ++logcounter);
    m_logLevel = m_settings->value("Logs/loglevel", "Info").toString();
    m_servicePort = m_settings->value("Main/serviceport", "502").toInt();
    m_reconnectPeriodInSec = m_settings->value("Client/reconnectperiod", "15").toInt();
    foreachConfFile();
}

void Settings::foreachConfFile()
{
    QDir dir(m_configDirectory + "conf.d");

    QStringList sl = dir.entryList(QStringList("*.conf"), QDir::NoFilter, QDir::SortFlag::Name);
    foreach (QString str, sl) {
        readDevSettings(dir.path() + "/" + str);
    }
}

void Settings::readDevSettings(const QString &confFile)
{
    bool ok;
    DeviceStruct device;

    QSettings *settings = new QSettings(confFile, QSettings::NativeFormat);
    device.ip = settings->value("IP", Defaults::ip).toString();
    QStringList sl = device.ip.split(".");
    if (sl.size() < 4) {
        qWarning() << "Device described in file " + confFile + " has wrong IP setting";
        return;
    }
    for (auto it = sl.begin(); it != sl.end(); ++it) {
        (*it).toUInt(&ok);
        if (!ok) {
            qWarning() << "Device described in file " + confFile + " has wrong IP setting";
            return;
        }
    }
    device.startPort = settings->value("Port").toUInt(&ok);
    if (!ok) {
        qWarning() << "Device described in file " + confFile + " has wrong Port setting";
        return;
    }
    for (int i = 1; i < Defaults::maxDevPortsCount; ++i) {
        PortStruct port;
        QString portname = "Port" + QString::number(i);
        port.use = (settings->value(portname, "No").toString() == "No") ? false : true;
        port.speed = settings->value(portname + "Speed", "115200").toUInt();
        port.timeout = settings->value(portname + "Timeout", "5").toUInt();
        QString str = settings->value(portname + "Parity", "None").toString();
        if (str == "None")
            port.parity = Parity::NONE;
        else if (str == "Even")
            port.parity = Parity::EVEN;
        else
            port.parity = Parity::ODD;
        port.stopbit = settings->value(portname + "Stop", "1").toUInt();
        device.ports += port;
    }
    m_deviceList += device;
}

void Settings::logSettings()
{
    Q_ASSERT(m_settings != nullptr);

    Logger::writeLog(Logger::All, "Reading settings from: " + m_settings->fileName());
    Logger::writeLog(Logger::All, "Startup information:");
    Logger::writeLog(Logger::All, "=========================");
    Logger::writeLog(Logger::All, "LogLevel: " + m_logLevel);
    Logger::writeLog(Logger::All, "Service Port: " + QString::number(m_servicePort));
    Logger::writeLog(Logger::All,
                     "Client reconnect period: " + QString::number(m_reconnectPeriodInSec));
    Logger::writeLog(Logger::All, "=========================");
}

void Settings::writeSettings()
{
    Q_ASSERT(m_settings != nullptr);
    m_settings->setValue("Logs/loglevel", m_logLevel);
    m_settings->setValue("Main/serviceport", QString::number(m_servicePort));
    m_settings->setValue("Client/reconnectperiod", QString::number(m_reconnectPeriodInSec));
    m_settings->sync();
}

int Settings::reconnectPeriod()
{
    return m_reconnectPeriodInSec;
}

QList<Settings::DeviceStruct> Settings::deviceList()
{
    return m_deviceList;
}

int Settings::servicePort()
{
    return m_servicePort;
}

QString Settings::loglevel()
{
    return m_logLevel;
}
