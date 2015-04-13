#include "cdtprocessorapplication.h"
#include <QtNetwork>
#include <QtXml>
#ifdef Q_OS_WIN//Windows GUI
#include <QtGui>
#endif

#include "cdtpluginloader.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"
#include "cdtautothresholdinterface.h"
#include "cdttaskmanager.h"
#include "cdttask.h"

QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;
QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

CDTProcessorApplication::CDTProcessorApplication(int &argc, char **argv) :
#ifdef Q_OS_WIN//Windows GUI
    QApplication(argc,argv),
#else
    QCoreApplication(argc,argv),
#endif
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
    connect(taskManager,SIGNAL(taskCompleted(QString,QByteArray)),SLOT(onTaskCompleted(QString,QByteArray)));
}

QString CDTProcessorApplication::getTempFileName(QString suffix)
{
    return QDir::tempPath() + "/" + QUuid::createUuid().toString() + suffix;
}

void CDTProcessorApplication::initPlugins()
{
    pbcdDiffPlugins      = CDTPluginLoader<CDTPBCDDiffInterface>::getPlugins();
    pbcdMergePlugins     = CDTPluginLoader<CDTPBCDMergeInterface>::getPlugins();
    autoThresholdPlugins =CDTPluginLoader<CDTAutoThresholdInterface>::getPlugins();
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
    if (flag != "CDTTask")
    {
        returnDebugMessage("Flag is not CDTTask");
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
    if (ret == false)
    {
        returnDebugMessage("setContent Failed");
        return;
    }

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

    taskManager->appendNewTask(taskName,taskID,doc);
}

void CDTProcessorApplication::returnDebugMessage(QString msg)
{
    QByteArray toReturn;
    QDataStream stream(&toReturn,QFile::ReadWrite);
    stream<<QString("Debug")<<msg;
    udpSender->writeDatagram(toReturn,QHostAddress::LocalHost,portDownload);
}

void CDTProcessorApplication::returnTaskInfo(QString id, CDTTaskInfo info)
{
    QByteArray toReturn;
    QDataStream stream(&toReturn,QFile::ReadWrite);
    stream<<QString("TaskInfo")<<id<<info.status<<info.currentStep<<info.currentProgress<<info.totalProgress;;
    udpSender->writeDatagram(toReturn,QHostAddress::LocalHost,portDownload);
}

void CDTProcessorApplication::returnTaskResult(QString id, QByteArray data)
{
    QByteArray toReturn;
    QDataStream stream(&toReturn,QFile::ReadWrite);
    stream<<QString("TaskResult")<<id<<data;
    udpSender->writeDatagram(toReturn,QHostAddress::LocalHost,portDownload);
}

void CDTProcessorApplication::onTaskAppended(QString id)
{
    Q_UNUSED(id);
}

void CDTProcessorApplication::onTaskInfoUpdated(QString id, CDTTaskInfo info)
{
    returnTaskInfo(id,info);
}

void CDTProcessorApplication::onTaskCompleted(QString id, QByteArray result)
{
    returnTaskResult(id,result);
}
