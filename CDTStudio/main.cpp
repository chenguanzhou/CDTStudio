#include "mainwindow.h"
#include "cdtapplication.h"

//TODO  File system
//TODO  Some QButtonGroup
//TODO  Project tree Checkbox
//TODO  Translation
//TODO  CDTBaseObject more independent
//TODO  Recent file
//TODO  Log System
//TODO  Comment
//TODO  Unit Test(Auto)
//TODO  ProgressBar Manager
//TODO  ORM
//TODO  Semi-auto segmentation
//TODO  Classification Assessment

//BUG   Same name
//BUG   PCA and transform information to show

int main(int argc, char *argv[])
{
    CDTApplication a(argc, argv);

    Log4Qt::BasicConfigurator::configure();
    Log4Qt::Logger::rootLogger()->info("Log4Qt is running!");

    QTranslator appTranslator;
    if (appTranslator.load(":/trans/" + QLocale::system().name()+".qm"))
    {
        a.installTranslator(&appTranslator);
        Log4Qt::Logger::rootLogger()->info(QString("Current translation file is %1!")
                .arg(":/trans/" + QLocale::system().name()+".qm"));
    }
    else
        Log4Qt::Logger::rootLogger()->warn("Load translation file failed!");


    MainWindow w;
    w.show();

    int ret = a.exec();

    return ret;
}
