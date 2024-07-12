#include <include/vdriver/commandlineparser.h>

#include <QCommandLineParser>
#include <QEventLoop>
#include <QTimer>
#include <gpiod.h>
#include <iostream>

CommandLineParser::CommandLineParser(QObject *parent) : QObject(parent)
{
}

bool CommandLineParser::parseCommandLine(Settings &settings)
{
    QCommandLineParser parser;

    parser.setApplicationDescription("Virtual Com Port Driver for i-7188e3/5");

    parser.addHelpOption();
    parser.addVersionOption();
    if (QCoreApplication::arguments().size() > 1)
    {
        parser.process(QCoreApplication::arguments());
        return false;
    }
    return true;
}

