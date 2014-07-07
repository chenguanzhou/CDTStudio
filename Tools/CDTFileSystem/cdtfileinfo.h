#ifndef CDTFILEINFO_H
#define CDTFILEINFO_H
#include <QtCore>

class CDTFileSystem;

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


#endif // CDTFILEINFO_H
