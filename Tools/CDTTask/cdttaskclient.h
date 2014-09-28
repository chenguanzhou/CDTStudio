#ifndef CDTTASKCLIENT_H
#define CDTTASKCLIENT_H

#include "cdttask_global.h"
#include <QObject>

class QUdpSocket;
class CDTTASKSHARED_EXPORT CDTTaskClient:public QObject
{
    Q_OBJECT
public:
    explicit CDTTaskClient(QObject *parent = NULL);

    bool sendNewTask(const QByteArray &data);
public slots:
    void readMessage();

private:
    QUdpSocket* udpReceiver;
    QUdpSocket* udpSender;
    qint16 portUpload;
    qint16 portDownload;
};

#endif // CDTTASKCLIENT_H
