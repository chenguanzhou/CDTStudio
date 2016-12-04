#ifndef CDTPLUGINLOADER_H
#define CDTPLUGINLOADER_H

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
            {
                plugins<<plugin;
            }
        }

//        QString className = plugins[0]->metaObject()->className();
        QString className = T::staticMetaObject.className();
        if (plugins.size()!=0)
            qDebug()<<QString("%1 plugins of %2 is initialized!").arg(plugins.size()).arg(className);
        else
            qWarning()<<QString("No plugin of %1 found!").arg(className);

        return plugins;
    }

private:
    static QDir directoryof(const QString &subDir)
    {
        QDir dir(QCoreApplication::applicationDirPath());
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
