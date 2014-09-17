#include "cdtprocessorapplication.h"
#include <QtNetwork>
#include <QtXml>

#include "cdtpluginloader.h"
#include "cdtpbcddiffinterface.h"
#include "cdttaskmanager.h"
#include "cdttask.h"

QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;

CDTProcessorApplication::CDTProcessorApplication(int &argc, char **argv) :
    QCoreApplication(argc,argv),
    udpReceiver(new QUdpSocket(this)),
    udpSender(new QUdpSocket(this)),
    taskManager(new CDTTaskManager(this))
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Settings");
    portUpload = setting.value("UpPort",59876).toInt();
    portDownload = setting.value("DownPort",59877).toInt();

    initPlugins();

    udpReceiver->bind(QHostAddress::LocalHost,portUpload);
    connect(udpReceiver,SIGNAL(readyRead()),SLOT(readCommand()));

    connect(taskManager,SIGNAL(taskAppended(QString)),SLOT(onTaskAppended(QString)));
    connect(taskManager,SIGNAL(taskInfoUpdated(QString,CDTTaskInfo)),SLOT(onTaskInfoUpdated(QString,CDTTaskInfo)));
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
    {
        returnDebugMessage("Flag is not CDTData");
        return;
    }

    in>>xmlContent;

    if (xmlContent.isEmpty())
    {
        returnDebugMessage("No xml content");
        return;
    }

    QDomDocument doc;
    bool ret = doc.setContent(xmlContent);

    returnDebugMessage(doc.toString(4));

    QDomElement root = doc.documentElement();

    if (root.isNull())
    {
        returnDebugMessage("Root element is null");
        return;
    }

    if (root.nodeName()!="CDTTask")
    {
        returnDebugMessage("No \"CDTTask\" in xml content");
        return;
    }

    QDomElement task = root.firstChildElement();
    if (task.isNull())
    {
        returnDebugMessage("No data in xml");
        return;
    }

    QString taskName = task.attribute("name");
    if (taskName.isEmpty())
    {
        returnDebugMessage("Task name is empty");
        return;
    }
    QUuid taskID    = QUuid(task.attribute("id"));
    if (taskID.isNull())
    {
        returnDebugMessage("Task id is invalid");
        return;
    }

    QDomElement params = task.firstChildElement("params");
    taskManager->appendNewTask(taskName,taskID,params);
}

void CDTProcessorApplication::returnDebugMessage(QString msg)
{
    QByteArray toReturn;
    QDataStream stream(&toReturn,QFile::ReadWrite);
    stream<<QString("Debug")<<msg;
    udpSender->writeDatagram(toReturn,QHostAddress::LocalHost,portDownload);
}

void CDTProcessorApplication::returnTaskInfo(CDTTaskInfo info)
{
    QByteArray toReturn;
    QDataStream stream(&toReturn,QFile::ReadWrite);
    stream<<QString("TaskInfo")<<info.status<<info.currentStep<<info.currentProgress<<info.totalProgress;;
    udpSender->writeDatagram(toReturn,QHostAddress::LocalHost,portDownload);
}

void CDTProcessorApplication::onTaskAppended(QString id)
{

}

void CDTProcessorApplication::onTaskInfoUpdated(QString id, CDTTaskInfo info)
{
    returnTaskInfo(info);
}
