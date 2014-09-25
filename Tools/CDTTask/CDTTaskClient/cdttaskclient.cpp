#include "cdttaskclient.h"
#include <QtNetwork>

CDTTaskClient::CDTTaskClient(QObject *parent)
    :QObject(parent),
    udpReceiver(new QUdpSocket(this)),
    udpSender(new QUdpSocket(this))
{
    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Settings");
    portUpload = setting.value("UpPort",59876).toInt();
    portDownload = setting.value("DownPort",59877).toInt();

    udpReceiver->bind(QHostAddress::LocalHost,portDownload);
    connect(udpReceiver,SIGNAL(readyRead()),SLOT(readMessage()));
}

bool CDTTaskClient::sendNewTask(const QByteArray &data)
{
    QByteArray toSend;
    QDataStream stream(&toSend,QFile::ReadWrite);
    stream<<QString("CDTData")<<data;
    if (udpSender->writeDatagram(toSend,QHostAddress::LocalHost,portUpload)==-1)
    {
        qWarning()<<"Upload failed!";
        return false;
    }
    return true;
}

void CDTTaskClient::readMessage()
{
    while (udpReceiver->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpReceiver->pendingDatagramSize());
        udpReceiver->readDatagram(datagram.data(), datagram.size());
        QDataStream in(datagram);
        int type;
        in>>type;
        if (type == CDTTask::Debug)
        {
            QString dbgText;
            in>>dbgText;
            qDebug()<<"Debug from server: "<<dbgText;
        }
        else if (type == CDTTask::TaskInfo)
        {

        }
//        QString flag,data;
//        in>>flag;
//        if (flag.toLower()=="debug")
//        {
//            in>>data;
//            qDebug()<<"Debug from server: "<<data;
//        }
//        else if (flag.toLower()=="taskinfo")
//        {
//            int status,currentProgress,totalProgress;
//            QString id,currentStep;
//            in>>id>>status>>currentStep>>currentProgress>>totalProgress;
//            emit taskInfoUpdated(id,status,currentStep,currentProgress,totalProgress);
//        }
//        else if (flag.toLower()=="taskresult")
//        {
//            QString id;
//            QByteArray result;
//            in>>id>>result;
//            emit taskInfoUpdated(id,2,tr("finished"),100,100);
//            emit taskCompleted(id,result);
//        }
    }
}
