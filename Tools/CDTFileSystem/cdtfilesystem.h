#ifndef CDTFILESYSTEM_H
#define CDTFILESYSTEM_H

#include "cdtfilesystem_global.h"
#include <QtCore>

class CDTFilePrivate;

class CDTFILESYSTEM_EXPORT CDTFileSystem
{
public:
    CDTFileSystem();
    ~CDTFileSystem();

    friend QDataStream &operator<<(QDataStream &out, const CDTFileSystem &files);
    friend QDataStream &operator>>(QDataStream &in, CDTFileSystem &files);

//    bool addFile(QStringList fileNameList,QUuid &id);
//    bool updateFile(QStringList fileNameList,QUuid &id);
//    QStringList getFile(const QUuid &id);// return files' name

private:
    void clear();

private:
    QMap<QUuid,QList<QFile*> > files;
};

//QDataStream CDTFILESYSTEM_EXPORT &operator<<(QDataStream &out, const CDTFileSystem &files);
//QDataStream CDTFILESYSTEM_EXPORT &operator>>(QDataStream &in, CDTFileSystem &files);


#endif // CDTFILESYSTEM_H
