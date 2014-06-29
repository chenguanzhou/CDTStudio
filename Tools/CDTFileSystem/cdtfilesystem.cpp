#include "cdtfilesystem.h"
#include <QtCore>
#include <gdal_priv.h>
#include <ogrsf_frmts.h>
#include <ogr_api.h>
#include "quazip.h"
#include "quazipfile.h"
#include "log4qt/basicconfigurator.h"

class CDTFileInfo
{
public:
    CDTFileInfo(
            QString pre = QString(),
            QString suf = QString(),
            QString filePath = QString(),
            QStringList affaffiliated = QStringList())
        :prifix(pre),suffix(suf),path(filePath),affiliatedFiles(affaffiliated) {}

    friend class CDTFileSystem;
    friend QDataStream &operator<<(QDataStream &out, const CDTFileInfo &fileInfo);
    friend QDataStream &operator>>(QDataStream &in, CDTFileInfo &fileInfo);

    friend QDataStream &operator<<(QDataStream &out, const CDTFileSystem &files);
    friend QDataStream &operator>>(QDataStream &in, CDTFileSystem &files);

    QString fullPath()const{return prifix+path+suffix;}
private:
    QString prifix;
    QString suffix;
    QString path;
    QStringList affiliatedFiles;
};

QDataStream &operator<<(QDataStream &out, const CDTFileInfo &fileInfo)
{
    out<<fileInfo.prifix<<fileInfo.suffix<<fileInfo.path<<fileInfo.affiliatedFiles;
    return out;
}

QDataStream &operator>>(QDataStream &in, CDTFileInfo &fileInfo)
{
    in>>fileInfo.prifix>>fileInfo.suffix>>fileInfo.path>>fileInfo.affiliatedFiles;
    return in;
}

class CDTFileSystemPrivate
{
    friend class CDTFileSystem;
    friend QDataStream &operator<<(QDataStream &out, const CDTFileSystem &files);
    friend QDataStream &operator>>(QDataStream &in, CDTFileSystem &files);

    QMap<QString,CDTFileInfo> files;
};

CDTFileSystem::CDTFileSystem(QObject *parent)
    : QObject(parent),
      pData(new CDTFileSystemPrivate)
{    
    Log4Qt::BasicConfigurator::configure();
}

CDTFileSystem::~CDTFileSystem()
{
    foreach (QString id, pData->files.keys()) {
        CDTFileInfo info = pData->files.value(id);
        QStringList fileToDelete;
        fileToDelete<<info.path;
        fileToDelete.append(info.affiliatedFiles);
        foreach (QString path, fileToDelete) {
            if (QFile(path).remove()==false)
                logger()->warn("Remove file:%1 failed!",path);
            else
                logger()->info("Remove file:%1 succeded!",path);
        }
        QDir::temp().rmdir(id);
    }
    delete pData;
}

bool CDTFileSystem::registerFile(QString id,
        const QString &filePath,
        QString prefix,
        QString suffix,
        QStringList affiliatedFiles)
{
    if (pData->files.contains(id))
    {
        logger()->error("FileID:%1 was exist in file system already!",id);
        return false;
    }
    if (!QFileInfo(filePath).isFile())
    {
        logger()->error("File:%1 was invalid!",filePath);
        return false;
    }
    pData->files.insert(id,CDTFileInfo(prefix,suffix,filePath,affiliatedFiles));
    return true;
}

bool CDTFileSystem::getFile(QString id, QString &filePath)
{
    if (pData->files.contains(id)==false)
    {
        logger()->error("FileID:%1 was not exist in file system!",id);
        return false;
    }
    filePath = pData->files.value(id).fullPath();
    return true;
}

bool CDTFileSystem::getFile(QString id, QString &filePath, QStringList &affiliatedFiles)
{
    if (pData->files.contains(id)==false)
    {
        logger()->error("FileID:%1 was not exist in file system!",id);
        return false;
    }
    filePath = pData->files.value(id).fullPath();
    affiliatedFiles = pData->files.value(id).affiliatedFiles;
    return true;
}

bool CDTFileSystem::GDALGetRasterVSIZipFile(const QString &srcPath, const QString &zipPath,bool deleteSrcFile)
{
    GDALAllRegister();
    GDALDataset *poDS = (GDALDataset *)GDALOpen(srcPath.toUtf8().constData(),GA_ReadOnly);
    if (poDS==NULL)
        return false;

    QuaZip zip(zipPath);
    zip.open(QuaZip::mdCreate);
    QuaZipFile outFile(&zip);

    char **fileList = poDS->GetFileList();
    GDALClose(poDS);

    int count = CSLCount(fileList);
    for(int i=0;i<count;++i)
    {
        QFileInfo fileInfo(QString::fromUtf8(fileList[i]));
        QFile inFile(srcPath);
        inFile.open(QFile::ReadOnly);
        outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileInfo.fileName(), fileInfo.filePath()));
        outFile.write(inFile.readAll());
        inFile.close();
        if (deleteSrcFile) inFile.remove();
        outFile.close();
    }

    CSLDestroy(fileList);
    return true;
}

bool CDTFileSystem::GDALGetShapefileVSIZipFile(const QString &srcPath, const QString &zipPath,bool deleteSrcFile)
{    
    OGRRegisterAll();
    QFileInfo info(srcPath);
    if (info.completeSuffix() != "shp")
        return false;

    QStringList suffixs;
    suffixs<<".shp"<<".shx"<<".dbf"<<".sbn"<<".sbx"<<".prj"<<".idm"<<".ind"<<".qix"<<".cpg";
    for (auto iter = suffixs.begin();iter!= suffixs.end();++iter)
        (*iter) = info.completeBaseName() + *iter;

    QDir dir = info.dir();
    QStringList files = dir.entryList(suffixs,QDir::Files);

    QuaZip zip(zipPath);
    zip.open(QuaZip::mdCreate);
    QuaZipFile outFile(&zip);
    foreach (QString file, files)
    {
        file = dir.absolutePath()+"/"+file;
        QFileInfo fileInfo(file);
        QFile inFile(file);
        inFile.open(QFile::ReadOnly);
        outFile.open(QIODevice::WriteOnly, QuaZipNewInfo(fileInfo.fileName(), fileInfo.filePath()));
        outFile.write(inFile.readAll());        
        inFile.close();
        if (deleteSrcFile) inFile.remove();
        outFile.close();
    }
    return true;
}

QStringList CDTFileSystem::GetShapefileAffaliated(const QString &srcPath)
{
    OGRRegisterAll();
    QFileInfo info(srcPath);
    if (info.completeSuffix() != "shp")
        return QStringList();

    QStringList suffixs;
    suffixs<<".shx"<<".dbf"<<".sbn"<<".sbx"<<".prj"<<".idm"<<".ind"<<".qix"<<".cpg";
    for (auto iter = suffixs.begin();iter!= suffixs.end();++iter)
        (*iter) = info.completeBaseName() + *iter;
    QDir dir = info.dir();
    QStringList files = dir.entryList(suffixs,QDir::Files);
    for(auto iter = files.begin() ; iter!=files.end();++iter)
        *iter = dir.absoluteFilePath(*iter);
    return files;
}

QDataStream &operator<<(QDataStream &out, const CDTFileSystem &files)
{
    out<<files.pData->files;    

    foreach (QString id, files.pData->files.keys()) {
        QStringList list;
        CDTFileInfo info = files.pData->files.value(id);        
        list<<info.path;
        foreach (QString affFilePath, info.affiliatedFiles) {
            list<<affFilePath;
        }

        foreach (QString path, list) {
            QFile file(path);
            if (!file.exists())
            {
                files.logger()->warn("File:%1 is not exist!",path);
                out<<QByteArray();
                continue;
            }
            if (!file.open(QFile::ReadOnly))
            {
                files.logger()->warn("Failed to open file:%1!",path);
                out<<QByteArray();
                continue;
            }
            out<<file.readAll();
        }
    }

    return out;
}

QDataStream &operator>>(QDataStream &in, CDTFileSystem &files)
{
    in>>files.pData->files;

    foreach (QString id, files.pData->files.keys()) {
        QStringList list;
        CDTFileInfo info = files.pData->files.value(id);
        QDir dir(QDir::temp());
        dir.mkdir(id);
        dir.cd(id);

        info.path = dir.absolutePath()+"/"+QFileInfo(info.path).fileName();
        list<<info.path;

        QStringList newAffList;
        foreach (QString affFilePath, info.affiliatedFiles) {
            QString filePath = dir.absolutePath()+"/"+QFileInfo(affFilePath).fileName();
            newAffList << filePath;
            list<<filePath;
        }

        info.affiliatedFiles = newAffList;
        files.pData->files.insert(id,info);

        foreach (QString path, list) {
            QByteArray data;
            in>>data;
            QFile file(path);
            file.open(QFile::WriteOnly);
            file.write(data);
        }


    }
    return in;
}
