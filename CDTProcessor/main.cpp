#include "cdtprocessorapplication.h"
#include <iostream>
#include <QtCore>

#ifdef Q_OS_WIN
#include <QMenu>
#include <QSystemTrayIcon>
#endif

QStringList getSystemProcessNames()
{
    QStringList result;
#ifdef Q_OS_WIN
    QString tempPath = QUuid::createUuid().toString()+".txt";
    system(QString("tasklist /FO CSV >>%1").arg(tempPath).toLocal8Bit().constData());
    QFile file(tempPath);
    file.open(QFile::ReadOnly);
    QTextStream in(&file);
    in.readLine();//ignore the first line
    while (!in.atEnd()) {
        QString line = in.readLine();
        if (line.isEmpty())
            continue;
        QString processName = line.split(",")[0];
        processName.remove("\"");
        processName.remove(".exe");
        result<<processName;
    }
    file.close();
    file.remove();
#endif
    return result;
}

int main(int argc, char *argv[])
{
    QStringList list = getSystemProcessNames();
    QString thisProcessName = argv[0];
    qDebug()<<list;
    qDebug()<<QFileInfo(thisProcessName).completeBaseName();
    if (list.count(QFileInfo(thisProcessName).completeBaseName())>1)
    {
        qCritical()<<"The Processor is exist";
        return 0;
    }

    CDTProcessorApplication a(argc, argv);

#ifdef Q_OS_WIN//Windows GUI SystemTrayIcon
    QSystemTrayIcon *systemTrayIcon  = new QSystemTrayIcon(QIcon(":/icons/processor.png"));
    systemTrayIcon->setToolTip(QObject::tr("CDT Processor(Server)"));
    systemTrayIcon->show();
    QMenu *menu = new QMenu();
    QAction *actionQuit = menu->addAction(QObject::tr("Exit"));
    QObject::connect(actionQuit,SIGNAL(triggered()),&a,SLOT(quit()));
    QObject::connect(&a,SIGNAL(lastWindowClosed()),systemTrayIcon,SLOT(hide()));
    systemTrayIcon->setContextMenu(menu);
#endif

    return a.exec();
}
