#ifndef CDTPROCESSORAPPLICATION_H
#define CDTPROCESSORAPPLICATION_H

#include <QCoreApplication>

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<CDTProcessorApplication *>(QCoreApplication::instance()))

class QUdpSocket;
class CDTProcessorApplication : public QCoreApplication
{
    Q_OBJECT
public:
    explicit CDTProcessorApplication(int &argc, char **argv);

private:
    void initPlugins();

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
