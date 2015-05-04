#ifndef CDTRECENTFILE_H
#define CDTRECENTFILE_H

#include <QObject>

class CDTRecentFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ files WRITE setFiles NOTIFY filesChanged)
public:
    explicit CDTRecentFile(QString keyName, QObject *parent = 0);
    ~CDTRecentFile();

    QStringList files()const;

signals:
    void filesChanged(QStringList);

public slots:    
    void addFile(QString file);
    void setFiles(QStringList f);

private:
    QString key;
    QStringList filesList;
};

#endif // CDTRECENTFILE_H
