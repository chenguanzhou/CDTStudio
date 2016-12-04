#include "mainwindow.h"
#include "cdtapplication.h"
#include <QObject>
#include <QDebug>

//TODO  Unit Test
//TODO  Comment => Document
//TODO  Settings

//TODO  Change Detection View
//TODO  Recent file
//TODO  Attribute Widget Closable(invisible)
//TODO  Read & write project in another thread
//TODO  FileSystem as a db
//TODO  Mask image process

int main(int argc, char *argv[])
{
#if QT_VERSION >= 0x050600
    QCoreApplication::setAttribute(Qt::AA_DisableHighDpiScaling,true);
#endif
    CDTApplication a(argc, argv);

    qDebug()<<QLocale::system().name();

    QTranslator appTranslator;
    if (appTranslator.load(":/Trans/" + QLocale::system().name()+".qm"))
    {
        a.installTranslator(&appTranslator);
        qDebug()<<QString("Current translation file is %1!")
                .arg(":/Trans/" + QLocale::system().name()+".qm");
    }
    else
        qWarning("Load translation file %1 failed!",
                                           ":/Trans/" + QLocale::system().name()+".qm");



    MainWindow w;
    w.show();

    int ret = a.exec();

    return ret;
}
