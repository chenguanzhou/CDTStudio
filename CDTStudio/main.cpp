#include "mainwindow.h"
#include <qgsapplication.h>
#include <QTranslator>
#include <QDebug>
#include <QDir>
#include <QtSql>
#include <QMessageBox>
#include "cdtpluginloader.h"
#include "cdtsegmentationInterface.h"
#include "cdtattributesinterface.h"
#include "cdtclassifierinterface.h"

QList<CDTSegmentationInterface *>   segmentationPlugins;
QList<CDTAttributesInterface *>     attributesPlugins;
QList<CDTClassifierInterface *>     classifierPlugins;

#ifdef Q_OS_WIN
#include <opencv2/core/version.hpp>
#define CV_VERSION_ID   \
    CVAUX_STR(CV_MAJOR_VERSION) CVAUX_STR(CV_MINOR_VERSION) CVAUX_STR(CV_SUBMINOR_VERSION)
#ifdef QT_DEBUG
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID "d"
#else
#define cvLIB(name) \
    "opencv_" name CV_VERSION_ID
#endif
#pragma comment(lib,cvLIB("core"))
#pragma comment(lib,cvLIB("imgproc"))
#pragma comment(lib,cvLIB("highgui"))
#pragma comment(lib,cvLIB("ml"))
#endif

bool initDatabase()
{
    /// Add database
    if (QSqlDatabase::drivers().contains("QSQLITE")==false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Can't find QSQLITE driver, this application couldn't run!"));
        return false;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","category");
    db.setDatabaseName(":memory:");
//    db.setDatabaseName("d:/hehe.db");
    if (!db.open())
    {
        QMessageBox::warning(NULL, QObject::tr("Unable to open database"),
                             QObject::tr("An error occurred while opening the connection: ")
                             + db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    bool ret ;

    ///  Create project table(id text,name text).
    ret = query.exec("CREATE TABLE project (id text NOT NULL, name text NOT NULL,Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table project failed!\nerror:")+query.lastError().text());
        return false;
    }

    ///  Create image layer table(id text,name text,path text,projectID text).
    ret = query.exec("CREATE TABLE imagelayer (id text NOT NULL, name text NOT NULL,path text NOT NULL,projectID text NOT NULL,Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table imagelayer failed!\nerror:")+query.lastError().text());
        return false;
    }

    ///  Create segmentation layer table(id text,name text,shapefilePath text,markfilePath text,imageID text).
    ret = query.exec("CREATE TABLE segmentationlayer"
                     "(id text NOT NULL, "
                     "name text NOT NULL,"
                     "shapefilePath text NOT NULL,"
                     "markfilePath text NOT NULL,"
                     "imageID text NOT NULL,"
                     "Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table segmentationlayer failed!\nerror:")+query.lastError().text());
        return false;
    }

    /// Create category table(name text,color blob,imageID text).
    ret = query.exec("CREATE TABLE category "
                     "(id text NOT NULL,"
                     "name text NOT NULL, "
                     "color blob, "
                     "imageID text NOT NULL,"
                     "Primary Key(id),"
                     "UNIQUE (name,imageID) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table category failed!\nerror:")+query.lastError().text());
        return false;
    }


    ///  Create segmentation sample_segmentation table(id text,name text,shapefilePath text,markfilePath text,imageID text).
    ret = query.exec("CREATE TABLE sample_segmentation"
                     "(id text NOT NULL, "
                     "name text NOT NULL, "
                     "segmentationid text NOT NULL,"
                     "Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table sample_segmentation failed!\nerror:")+query.lastError().text());
        return false;
    }

    ///  Create segmentation samples table(id text,name text,shapefilePath text,markfilePath text,imageID text).
    ret = query.exec("CREATE TABLE samples"
                     "(objectid text NOT NULL, "
                     "categoryid text NOT NULL,"
                     "sampleid text NOT NULL,"
                     "Primary Key(objectid,categoryid,sampleid) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table sample_segmentation failed!\nerror:")+query.lastError().text());
        return false;
    }

    return true;
}

int main(int argc, char *argv[])
{
    QgsApplication a(argc, argv,true);

    QTranslator appTranslator;
    appTranslator.load(":/trans/" + QLocale::system().name()+".qm");
    a.installTranslator(&appTranslator);

    QgsApplication::setPluginPath(QDir::currentPath()+"/Plugins");
    QgsApplication::initQgis();

    if (initDatabase()==false)
        return 0;

    MainWindow w;
    w.showMaximized();

    segmentationPlugins = CDTPluginLoader<CDTSegmentationInterface>::getPlugins();
    attributesPlugins   = CDTPluginLoader<CDTAttributesInterface>::getPlugins();
    classifierPlugins   = CDTPluginLoader<CDTClassifierInterface>::getPlugins();

    return a.exec();
}
