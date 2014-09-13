#include "cdtprocessorapplication.h"
#include <QtNetwork>

#include "cdtpluginloader.h"
#include "cdtpbcddiffinterface.h"
QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;

CDTProcessorApplication::CDTProcessorApplication(int &argc, char **argv) :
    QCoreApplication(argc,argv),
    udpReceiver(new QUdpSocket(this)),
    udpSender(new QUdpSocket(this))
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Settings");
    port = setting.value("UpPort",59876).toInt();

    initPlugins();

    udpReceiver->bind(QHostAddress::LocalHost,port);
    connect(udpReceiver,SIGNAL(readyRead()),SLOT(readCommand()));

}

void CDTProcessorApplication::initPlugins()
{
    pbcdDiffPlugins     = CDTPluginLoader<CDTPBCDDiffInterface>::getPlugins();
    qDebug()<<pbcdDiffPlugins.size();
}

void CDTProcessorApplication::readCommand()
{
    while (udpReceiver->hasPendingDatagrams()) {
        QHostAddress ip;
        QByteArray datagram;
        datagram.resize(udpReceiver->pendingDatagramSize());
        udpReceiver->readDatagram(datagram.data(), datagram.size(),&ip);
        parseCommand(datagram);
    }
}

void CDTProcessorApplication::parseCommand(QByteArray data)
{
    if (data.isEmpty())
        return;

    QDataStream in(data);
    QString flag;
    QByteArray xmlContent;
    in>>flag;
    if (flag != "CDTData")
        return;

    in>>xmlContent;

    if (xmlContent.isEmpty())
        return;
}
