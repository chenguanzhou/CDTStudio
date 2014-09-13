#ifndef CDTPROCESSORAPPLICATION_H
#define CDTPROCESSORAPPLICATION_H

#include <QCoreApplication>

class QUdpSocket;
class CDTProcessorApplication : public QCoreApplication
{
    Q_OBJECT
public:
    explicit CDTProcessorApplication(int &argc, char **argv);

private:
    initPlugins();

signals:

public slots:
    void readCommand();
    void parseCommand(QByteArray data);

private:
    qint16 port;
    QUdpSocket *udpReceiver;
    QUdpSocket *udpSender;
};

#endif // CDTPROCESSORAPPLICATION_H
