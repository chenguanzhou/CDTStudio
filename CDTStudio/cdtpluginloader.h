#ifndef CDTPLUGINLOADER_H
#define CDTPLUGINLOADER_H

#include <QDir>
#include <QtCore>

template <typename T>
class CDTPluginLoader
{
public:
    static QList<T*> getPlugins()
    {
        QDir pluginDir=directoryof("Plugins");
        QList<T*> plugins;
        foreach (QString fileName, pluginDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
            if (T* plugin=qobject_cast<T*>(loader.instance()))
                plugins<<plugin;
        }

        return plugins;
    }

private:
    static QDir directoryof(const QString &subDir)
    {
        QDir dir(QApplication::applicationDirPath());

#if defined(Q_OS_WIN)
        if (dir.dirName().toLower()=="debug"||dir.dirName().toLower()=="release")
            dir.cdUp();
#elif defined(Q_OS_MAC)
        if (dir.dirName()=="MacOS")
        {
            dir.cdUp();dir.cdUp();dir.cdUp();
        }
#endif
        dir.cd(subDir);
        return dir;
    }
};

#endif // CDTPLUGINLOADER_H
