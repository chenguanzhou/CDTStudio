#include "mainwindow.h"
#include "cdtapplication.h"
#include <QObject>

//TODO  Unit Test
//TODO  Comment => Document
//TODO  Translation
//TODO  About
//TODO  Settings

//TODO  Change Detection View
//TODO  Recent file
//TODO  Attribute Widget Closable(invisible)
//TODO  Read & write project in another thread
//TODO  FileSystem as a db
//TODO  More statistics value for attributes' histogram
//TODO  Mask image process
//TODO  Relative dir in CDTImageLayer

int main(int argc, char *argv[])
{
    CDTApplication a(argc, argv);

    Log4Qt::BasicConfigurator::configure();
    Log4Qt::Logger::rootLogger()->info("Log4Qt is running!");

    QTranslator appTranslator;
    if (appTranslator.load(":/Trans/" + QLocale::system().name()+".qm"))
    {
        a.installTranslator(&appTranslator);
        Log4Qt::Logger::rootLogger()->info(QString("Current translation file is %1!")
                .arg(":/Trans/" + QLocale::system().name()+".qm"));
    }
    else
        Log4Qt::Logger::rootLogger()->warn("Load translation file %1 failed!",
                                           ":/Trans/" + QLocale::system().name()+".qm");



    MainWindow w;
    w.show();

    int ret = a.exec();

    return ret;
}
