#include "cdtrecentfile.h"
#include "stable.h"

CDTRecentFile::CDTRecentFile(QString keyName, QObject *parent)
    : QObject(parent),key(keyName)
{
    QSettings setting;
    setting.beginGroup(key);
    setFiles(setting.value("recentFiles").toStringList());
    setting.endGroup();
}

CDTRecentFile::~CDTRecentFile()
{
    QSettings setting;
    setting.beginGroup(key);
    setting.setValue("recentFiles",filesList);
    setting.endGroup();
}

QStringList CDTRecentFile::files() const
{
    return filesList;
}

///
/// \brief CDTRecentFile::addFile
/// \param file the new recently opened file
///
void CDTRecentFile::addFile(QString file)
{
    file = QFileInfo(file).absoluteFilePath();
    QStringList oriList = files();
    if (oriList.isEmpty())//Original list is empty
        oriList<<file;
    else
    {
        if (oriList.contains(file))//New file already exist in the list
            oriList.removeAll(file);
        oriList.insert(0,file);
    }
    setFiles(oriList);
}

void CDTRecentFile::setFiles(QStringList f)
{
    if (f==filesList)
        return;
    filesList = f;
    emit filesChanged(filesList);
}

