#ifndef CDTFILESYSTEM_H
#define CDTFILESYSTEM_H

#include "cdtfilesystem_global.h"
#include <QtCore>
#include "log4qt/logger.h"

class CDTFileSystemPrivate;

class CDTFILESYSTEM_EXPORT CDTFileSystem:public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
public:
    CDTFileSystem(QObject* parent = NULL);
    ~CDTFileSystem();

    friend QDataStream CDTFILESYSTEM_EXPORT &operator<<(QDataStream &out, const CDTFileSystem &files);
    friend QDataStream CDTFILESYSTEM_EXPORT &operator>>(QDataStream &in, CDTFileSystem &files);

    bool registerFile(
            QString id,
            const QString &filePath,
            QString prefix = QString(),
            QString suffix = QString());

    bool getFile(QString id,QString& filePath);

private:
    CDTFileSystemPrivate* pData;
};

//QDataStream CDTFILESYSTEM_EXPORT &operator<<(QDataStream &out, const CDTFileSystem &files);
//QDataStream CDTFILESYSTEM_EXPORT &operator>>(QDataStream &in, CDTFileSystem &files);


#endif // CDTFILESYSTEM_H
