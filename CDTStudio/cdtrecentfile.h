#ifndef CDTRECENTFILE_H
#define CDTRECENTFILE_H

#include <QObject>

class CDTRecentFile : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QStringList files READ files WRITE setFiles NOTIFY filesChanged)
public:
    explicit CDTRecentFile(const QString &keyName, QObject *parent = Q_NULLPTR);
    ~CDTRecentFile();

    QStringList files()const;

signals:
    void filesChanged(QStringList);

public slots:    
    void addFile(QString file);
    void setFiles(const QStringList &f);

private:
    QString key;
    QStringList filesList;
};

#endif // CDTRECENTFILE_H
