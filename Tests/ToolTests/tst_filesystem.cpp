#include <QString>
#include <QtTest>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include "cdtfilesystem.h"

class FileSystem : public QObject
{
    Q_OBJECT

public:
    FileSystem();

private slots:
    void initTestCase();

    //Tests
    void testRegister_data();
    void testRegister();
    void testIO();
    void testAffiliatedFiles();

    void cleanupTestCase();
private:
    CDTFileSystem *fileSystem;
};

FileSystem::FileSystem()    
{
    fileSystem = new CDTFileSystem();
}

void FileSystem::initTestCase()
{
    QString data = "I love GIS";

    //1.File in tempDir
    QString path = QDir::tempPath()+"/"+"FileSystem.txt";
    QFile file(path);
    file.open(QFile::WriteOnly);
    file.seek(0);
    QTextStream stream(&file);
    stream<<data;
    file.close();

    //2.File in other dir
    path = "FileSystem.txt";
    QFile otherFile(path);
    otherFile.open(QFile::Append);
    QTextStream otherstream(&otherFile);
    otherstream<<data;
    otherFile.close();
}

void FileSystem::testRegister_data()
{
    QTest::addColumn<QString>("id");
    QTest::addColumn<QString>("path");
    QTest::addColumn<QString>("prefix");
    QTest::addColumn<QString>("suffix");
    QTest::addColumn<QString>("fullPath");

    QTest::newRow("tempFile")
            <<QUuid::createUuid().toString()<< QDir::tempPath()+"/"+"FileSystem.txt"
           <<QString()<<QString()<<QDir::tempPath()+"/"+"FileSystem.txt";
    QTest::newRow("other dir")
            <<QUuid::createUuid().toString()<< "FileSystem.txt"
           <<QString()<<QString()<<"FileSystem.txt";
    QTest::newRow("prefix&suffix")
            <<QUuid::createUuid().toString()<< "FileSystem.txt"
           <<"/vsizip/"<<"/test.shp"<<"/vsizip/FileSystem.txt/test.shp";
}

void FileSystem::testRegister()
{
    QFETCH(QString, id);
    QFETCH(QString, path);
    QFETCH(QString, prefix);
    QFETCH(QString, suffix);
    QFETCH(QString, fullPath);


    fileSystem->registerFile(id,path,prefix,suffix);

    QString resultPath;
    bool ret = fileSystem->getFile(id,resultPath);
    QVERIFY(ret);
    QCOMPARE(resultPath,fullPath);
}

void FileSystem::testIO()
{
    CDTFileSystem *oldFileSystem;
    oldFileSystem = new CDTFileSystem();
    QStringList ids;
    ids<<QUuid::createUuid().toString()<<QUuid::createUuid().toString();
    oldFileSystem->registerFile(ids[0],QDir::tempPath()+"/"+"FileSystem.txt");
    oldFileSystem->registerFile(ids[1],"FileSystem.txt");

    QFile file("store.dat");
    bool ret = file.open(QFile::WriteOnly);
    QVERIFY(ret);
    QDataStream stream(&file);
    stream<<*oldFileSystem;
    file.close();

    CDTFileSystem *newFileSystem = new CDTFileSystem;
    ret = file.open(QFile::ReadOnly);
    QVERIFY(ret);
    QDataStream openStream(&file);
    openStream>>*newFileSystem;
    file.close();

    foreach (QString id, ids) {
        QString path;
        ret = newFileSystem->getFile(id,path);
        QVERIFY(ret);
        QFile textFile(path);
        ret = textFile.open(QFile::ReadOnly);
        QVERIFY(ret);
        QTextStream stream(&textFile);
        QCOMPARE(stream.readAll(),QString("I love GIS"));
        textFile.close();
    }
}

void FileSystem::testAffiliatedFiles()
{
    QFile file("main.dat");
    file.open(QFile::WriteOnly);
    QTextStream testStream(&file);
    testStream<<"main";
    file.close();

    QStringList affList = QStringList()<<"aff1.dat"<<"aff2.dat";
    foreach (QString path, affList) {
        QFile file(path);
        file.open(QFile::WriteOnly);
        QTextStream stream(&file);
        stream<<"affaliate";
        file.close();
    }

    CDTFileSystem *fileSystem = new CDTFileSystem();
    QString id = QUuid::createUuid().toString();
    fileSystem->registerFile(id,"main.dat",QString(),QString(),affList);

    QTemporaryFile store;
    store.open();
    QDataStream stream(&store);
    stream<<*fileSystem;
    store.flush();
    store.seek(0);
    delete fileSystem;

    stream.setDevice(&store);
    fileSystem = new CDTFileSystem();
    stream>>*fileSystem;
    store.close();

    QString newPath;
    QStringList newList;
    fileSystem->getFile(id,newPath,newList);

    file.setFileName(newPath);
    file.open(QFile::ReadOnly);
    testStream.setDevice(&file);
    QString text;
    testStream>>text;
    QVERIFY(QString("main")==text);
    file.close();
}

void FileSystem::cleanupTestCase()
{
    if (fileSystem) delete fileSystem;
    QFile("store.dat").remove();
}

QTEST_APPLESS_MAIN(FileSystem)

#include "tst_filesystem.moc"
