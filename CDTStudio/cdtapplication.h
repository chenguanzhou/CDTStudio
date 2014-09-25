#ifndef CDTAPPLICATION_H
#define CDTAPPLICATION_H

#include "QtGlobal"
#include "qgsapplication.h"
#include "cdttaskdockwidget.h"

class CDTApplication;
class QProcess;
class QUdpSocket;

#if defined(qApp)
#undef qApp
#endif
#define qApp (static_cast<CDTApplication *>(QCoreApplication::instance()))


class CDTApplication : public QgsApplication
{
    Q_OBJECT
public:
    explicit CDTApplication(int & argc, char ** argv);
    ~CDTApplication();

    static QString getStyleSheetByName(QString styleName);
signals:
    void taskInfoUpdated(QString id,int status,QString currentStep,int currentProgress,int totalProgress);
    void taskCompleted(QString id,QByteArray result);

public slots:
    void sendTask(const QByteArray &data);
    void readMessage();

private:
    void initPlugins();
    bool initDatabase();
    #ifdef Q_OS_WIN
    void initStxxl();
    #endif

    QProcess *processor;
    QUdpSocket* udpReceiver;
    QUdpSocket* udpSender;
    qint16 portUpload;
    qint16 portDownload;    
};

#endif // CDTAPPLICATION_H
