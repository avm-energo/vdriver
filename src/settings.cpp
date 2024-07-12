#include <QtDebug>
#include "qforeach.h"
#include <gen/files.h>
#include <gen/logger.h>
#include <include/vdriver/defaults.h>
#include <include/vdriver/settings.h>

Settings::Settings() {}

void Settings::init(QString &logFileName)
{
    m_settings = new QSettings(Defaults::settingsFileName, QSettings::NativeFormat);
    logFilename = m_settings->value("Logs/logfile", Defaults::logFileName).toString();
}

void Settings::readSettings()
{
    int logcounter = m_settings->value("Test/counter", "1").toInt();
    m_settings->setValue("Test/counter", ++logcounter);
    logLevel = m_settings->value("Logs/loglevel", "Info").toString();
    servicePort = m_settings->value("Main/serviceport", "502").toInt();
    foreachConfFile();
}

void Settings::foreachConfFile()
{
    QDir dir(Defaults::configDirectory);

    QStringList sl = dir.entryList(QStringList("*.conf"));
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
    for (int i = 0; i < Defaults::maxDevPortsCount; ++i) {
        PortStruct port;
        QString portname = "Port" + QString::number(i);
        port.use = (settings->value(portname, "No").toString() == "No") ? false : true;
        port.speed = settings->value(portname + "Speed", "115200").toUInt();
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
    Logger::writeLog(Logger::All, "LogLevel: " + logLevel);
    Logger::writeLog(Logger::All, "Service Port: " + QString::number(servicePort));
}

void Settings::writeSettings()
{
    Q_ASSERT(m_settings != nullptr);
    m_settings->setValue("Logs/loglevel", logLevel);
    m_settings->setValue("Main/serviceport", QString::number(servicePort));
    m_settings->setValue("Main/devicecount", QString::number(m_deviceList.size()));
    m_settings->sync();
}
