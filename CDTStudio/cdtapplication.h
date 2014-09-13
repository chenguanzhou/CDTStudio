#ifndef CDTAPPLICATION_H
#define CDTAPPLICATION_H

#include "QtGlobal"
#include "qgsapplication.h"

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<CDTApplication *>(QCoreApplication::instance()))

class QProcess;
class QUdpSocket;
class CDTApplication : public QgsApplication
{
    Q_OBJECT
public:
    explicit CDTApplication(int & argc, char ** argv);
    ~CDTApplication();

    static QString getStyleSheetByName(QString styleName);
signals:

public slots:

private:
    void initPlugins();
    bool initDatabase();
    #ifdef Q_OS_WIN
    void initStxxl();
    #endif

    QProcess *processor;
    QUdpSocket* udpReceiver;
    QUdpSocket* udpSender;
};

#endif // CDTAPPLICATION_H
