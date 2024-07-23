#pragma once

#include <QTimer>
#include <QObject>
#include <include/vdriver/tcpclient.h>

class DeviceParser : public QObject
{
    Q_OBJECT
public:
    explicit DeviceParser(TCPClient *client, QObject *parent = nullptr);

    void setName(const QString &name);

signals:
    void writeToTCP(const QByteArray &ba);
    void writeToRTU(const QByteArray &ba);

public slots:
    void newDataReceivedFromTCP(QByteArray &ba);
    void newDataReceivedFromRTU(QByteArray &ba);

private:
    QString m_name;
    TCPClient *m_client;
    QTimer *m_RTUTimer;
    QByteArray bin;

private slots:
    void RTUTimerTimeout();
    unsigned short   CalcCRC(QByteArray &ba );
};
