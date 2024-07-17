#ifndef SERVICEPARSER_H
#define SERVICEPARSER_H

#include <QObject>

class ServiceParser : public QObject
{
    Q_OBJECT
public:
    explicit ServiceParser(QObject *parent = nullptr);

signals:
    void writeToSocket(const QByteArray &ba);

public slots:
    void newDataReceivedFromSocket(QByteArray &ba);
};

#endif // SERVICEPARSER_H
