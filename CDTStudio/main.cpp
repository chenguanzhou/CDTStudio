#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QDir>
#include "cdtpluginloader.h"
#include "cdtsegmentationInterface.h"
#include "cdtattributesinterface.h"

QList<CDTSegmentationInterface *>   segmentationPlugins;
QList<CDTAttributesInterface *>     attributesPlugins;

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QTranslator appTranslator;
    appTranslator.load(":/trans/" + QLocale::system().name()+".qm");
    a.installTranslator(&appTranslator);

    MainWindow w;
    w.showMaximized();    

    segmentationPlugins = CDTPluginLoader<CDTSegmentationInterface>::getPlugins();
    attributesPlugins   = CDTPluginLoader<CDTAttributesInterface>::getPlugins();

    return a.exec();
}
