#ifndef CDTPLUGINLOADER_H
#define CDTPLUGINLOADER_H

#include "stable.h"

template <typename T>
class CDTPluginLoader
{
public:
    static QList<T*> getPlugins()
    {
        QDir pluginDir=directoryof("Plugins");
        QList<T*> plugins;

        QString className = typeid(T).name();

        foreach (QString fileName, pluginDir.entryList(QDir::Files))
        {
            QPluginLoader loader(pluginDir.absoluteFilePath(fileName));
            if (T* plugin=qobject_cast<T*>(loader.instance()))
            {
                plugins<<plugin;
            }
        }

        if (plugins.size()!=0)
            Log4Qt::Logger::rootLogger()->info("%1 plugins of %2 is initialized!",
                            plugins.size(),className);
        else
            Log4Qt::Logger::rootLogger()->warn("No plugin of %1 found!",className);

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
