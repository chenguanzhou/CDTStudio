#include "cdtapplication.h"
#include "stable.h"
#include <stxxl.h>
#include "cdtpluginloader.h"
#include "cdtsegmentationinterface.h"
#include "cdtattributesinterface.h"
#include "cdtclassifierinterface.h"
#include "cdtextractioninterface.h"
#include "cdtpbcddiffinterface.h"
#include "cdtpbcdmergeinterface.h"
#include "cdtautothresholdinterface.h"

QList<CDTSegmentationInterface *>   segmentationPlugins;
QList<CDTAttributesInterface *>     attributesPlugins;
QList<CDTClassifierInterface *>     classifierPlugins;
QList<CDTExtractionInterface *>     extractionPlugins;

QList<CDTPBCDDiffInterface *>       pbcdDiffPlugins;
QList<CDTPBCDMergeInterface *>      pbcdMergePlugins;
QList<CDTAutoThresholdInterface *>  autoThresholdPlugins;

CDTApplication::CDTApplication(int & argc, char ** argv) :
    QgsApplication(argc, argv,true),
    processor (new QProcess(this)),
    udpReceiver(new QUdpSocket(this)),
    udpSender(new QUdpSocket(this))
{
    setApplicationName("CDTStudio");
    setApplicationVersion("v0.1");

    QStringList env = QProcess::systemEnvironment();
    foreach (QString path, env) {
        if(path.startsWith("QGIS_LOG_FILE=",Qt::CaseInsensitive))
        {
            QString filePath = path.remove("QGIS_LOG_FILE=");
            qDebug()<<path;
            QFile(filePath).remove();
        }
    }

    QgsApplication::setPluginPath(QDir::currentPath()+"/Plugins");
    QgsApplication::initQgis();

    if (initDatabase()==false)
        exit(0);

#ifdef Q_OS_WIN
    initStxxl();
#endif

    initPlugins();

    this->setStyleSheet(getStyleSheetByName("default"));

    QSettings setting("WHU","CDTStudio");
    setting.beginGroup("Settings");
    portUpload = setting.value("UpPort",59876).toInt();
    portDownload = setting.value("DownPort",59877).toInt();

    udpReceiver->bind(QHostAddress::LocalHost,portDownload);
    connect(udpReceiver,SIGNAL(readyRead()),SLOT(readMessage()));

    processor->setStandardOutputFile(qApp->applicationDirPath()+"/task.log");
    processor->setStandardErrorFile(qApp->applicationDirPath()+"/error.log");
    processor->start("Processor");
}

CDTApplication::~CDTApplication()
{
    QSqlDatabase db = QSqlDatabase::database("category");
    QString databaseName = db.databaseName();
    db.removeDatabase("category");
    if (databaseName != ":memory:")
    {
        QFile::remove(databaseName);
    }
}

QString CDTApplication::getStyleSheetByName(QString styleName)
{
    styleName = styleName.toLower();
    QString styleSheet;
    if (styleName == "default")
    {
        QFile file(":/StyleSheet/default.css");
        if (file.open(QFile::ReadOnly))
        {
            QTextStream stream(&file);
            styleSheet = stream.readAll();
        }
        file.close();
    }
//    qDebug()<<styleSheet;
    return styleSheet;
}

void CDTApplication::sendTask(const QByteArray &data)
{
    QByteArray toSend;
    QDataStream stream(&toSend,QFile::ReadWrite);
    stream<<QString("CDTTask")<<data;
    if (udpSender->writeDatagram(toSend,QHostAddress::LocalHost,portUpload)==-1)
    {
        qWarning()<<"Upload failed!";
    }
}

void CDTApplication::readMessage()
{
    while (udpReceiver->hasPendingDatagrams()) {
        QByteArray datagram;
        datagram.resize(udpReceiver->pendingDatagramSize());
        udpReceiver->readDatagram(datagram.data(), datagram.size());
        QDataStream in(datagram);
        QString flag,data;
        in>>flag;
        if (flag.toLower()=="debug")
        {
            in>>data;
            qDebug()<<"Debug from server: "<<data;
        }
        else if (flag.toLower()=="taskinfo")
        {
            int status,currentProgress,totalProgress;
            QString id,currentStep;
            in>>id>>status>>currentStep>>currentProgress>>totalProgress;
            emit taskInfoUpdated(id,status,currentStep,currentProgress,totalProgress);
        }
        else if (flag.toLower()=="taskresult")
        {
            QString id;
            QByteArray result;
            in>>id>>result;
            emit taskInfoUpdated(id,2,tr("finished"),100,100);
            emit taskCompleted(id,result);
        }
    }
}

void CDTApplication::initPlugins()
{
    segmentationPlugins = CDTPluginLoader<CDTSegmentationInterface>::getPlugins();
    attributesPlugins   = CDTPluginLoader<CDTAttributesInterface>::getPlugins();
    classifierPlugins   = CDTPluginLoader<CDTClassifierInterface>::getPlugins();
    extractionPlugins   = CDTPluginLoader<CDTExtractionInterface>::getPlugins();

    pbcdDiffPlugins     = CDTPluginLoader<CDTPBCDDiffInterface>::getPlugins();
    pbcdMergePlugins    = CDTPluginLoader<CDTPBCDMergeInterface>::getPlugins();
    autoThresholdPlugins= CDTPluginLoader<CDTAutoThresholdInterface>::getPlugins();
}

bool CDTApplication::initDatabase()
{
    /// Add database
    if (QSqlDatabase::drivers().contains("QSQLITE")==false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Can't find QSQLITE driver, this application couldn't run!"));
        return false;
    }
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE","category");

    QString dbPath;
    dbPath = ":memory:";
    //    dbPath = "C:/Users/cgz/Documents/data/lalalla.db";

    //    QTemporaryFile dbFile;
    //    dbFile.open();
    //    dbPath = dbFile.fileName();
    //    dbFile.close();

    Log4Qt::Logger::rootLogger()->info("Program database is %1!",dbPath);

    db.setDatabaseName(dbPath);
    if (!db.open())
    {
        QMessageBox::warning(NULL, QObject::tr("Unable to open database"),
                             QObject::tr("An error occurred while opening the connection: ")
                             + db.lastError().text());
        Log4Qt::Logger::rootLogger()->warn("Program db open failed!\nerror msg:%1",db.lastError().text());
        return false;
    }

    QSqlQuery query(db);
    bool ret ;

    ///  Create project table(id text,name text).
    ret = query.exec("CREATE TABLE project (id text NOT NULL, name text NOT NULL,Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table project failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table project failed!\nerror msg:%1",query.lastError().text());
        return false;
    }

    ///  Create image layer table(id text,name text,path text,projectID text).
    ret = query.exec("CREATE TABLE imagelayer (id text NOT NULL, name text NOT NULL,path text NOT NULL,projectID text NOT NULL,Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table imagelayer failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table imagelayer failed!\nerror msg:%1",query.lastError().text());
        return false;
    }

    ///  Create segmentation layer table(id text,name text,shapefilePath text,markfilePath text,imageID text).
    ret = query.exec("CREATE TABLE extractionlayer"
                     "(id text NOT NULL, "
                     "name text NOT NULL,"
                     "shapefilePath text NOT NULL,"
                     "color blob,"
                     "borderColor blob,"
                     "opacity double,"
                     "imageID text NOT NULL,"
                     "Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table extractionlayer failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table extractionlayer failed!\nerror msg:%1",query.lastError().text());
        return false;
    }

    ///  Create segmentation layer table(id text,name text,shapefilePath text,markfilePath text,imageID text).
    ret = query.exec("CREATE TABLE segmentationlayer"
                     "(id text NOT NULL, "
                     "name text NOT NULL,"
                     "shapefilePath text NOT NULL,"
                     "markfilePath text NOT NULL,"
                     "method text NOT NULL,"
                     "params blob,"
                     "dbUrl blob,"
                     "bordercolor blob,"
                     "imageID text NOT NULL,"
                     "Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table segmentationlayer failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table segmentationlayer failed!\nerror msg:%1",query.lastError().text());
        return false;
    }

    ///  Create classification layer table(id text,name text,data blob,clsinfo blob,segmentationID text).
    ret = query.exec("CREATE TABLE classificationlayer"
                     "(id text NOT NULL, "
                     "name text NOT NULL,"
                     "method text NOT NULL,"
                     "params blob,"
                     "data blob,"
                     "clsinfo blob,"
                     "normalizeMethod text,"
                     "pca text,"
                     "segmentationID text NOT NULL,"
                     "Primary Key(id) )");
    if (ret == false)
    {
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table classificationlayer failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table classificationlayer failed!\nerror msg:%1",query.lastError().text());
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
        Log4Qt::Logger::rootLogger()->error("create table category failed!\nerror msg:%1",query.lastError().text());
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
        Log4Qt::Logger::rootLogger()->error("create table sample_segmentation failed!\nerror msg:%1",query.lastError().text());
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
        QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("create table samples failed!\nerror:")+query.lastError().text());
        Log4Qt::Logger::rootLogger()->error("create table samples failed!\nerror msg:%1",query.lastError().text());
        return false;
    }

    Log4Qt::Logger::rootLogger()->info("Program database initialized!");
    return true;
}

#ifdef Q_OS_WIN
void CDTApplication::initStxxl()
{
    QString stxxlFilePath = QDir::tempPath()+"\\cdtstudio_stxxl";
    stxxl::config * cfg = stxxl::config::get_instance();
    stxxl::disk_config disk(stxxlFilePath.toLocal8Bit().constData(), /*800 * 1024 * 1024*/0, "wincall delete");
    cfg->add_disk(disk);
    Log4Qt::Logger::rootLogger()->info("Stxxl file path is %1",stxxlFilePath);
}
#endif
