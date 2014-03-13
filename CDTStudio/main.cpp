#include "mainwindow.h"
#include <QApplication>
#include <QTranslator>
#include <QDebug>
#include <QDir>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
//    a.setLibraryPaths(a.libraryPaths()<<(QDir::currentPath()+"/plugins"));
//    qDebug()<<a.libraryPaths();
    QTranslator appTranslator;
    appTranslator.load(":/trans/" + QLocale::system().name()+".qm");
    a.installTranslator(&appTranslator);
    MainWindow w;
    w.showMaximized();    
    return a.exec();
}
