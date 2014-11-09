#ifndef CDTTABLEEXPORTER_H
#define CDTTABLEEXPORTER_H

#include "cdttableexporter_global.h"
#include <QObject>
#include <QMap>
#include <QSqlDatabase>

class CDTTableExporterWriter
{
public:
    CDTTableExporterWriter(const QString &path)
        :filePath(path),valid(true){

    }
    virtual ~CDTTableExporterWriter(){}
    bool isValid()const{return valid;}

    virtual void writeLine(QStringList record) = 0;
protected:
    QString filePath;
    bool valid;
};


class CDTTABLEEXPORTER_EXPORT CDTTableExporter
{
public:
    static bool exportSingleTable(QSqlDatabase &db,const QString &tableName,const QString &exportPath,bool isHeader,QString &error);
    static bool exportMultiTables(QSqlDatabase &db,const QStringList &tableNames,const QString &exportPath,bool isHeader,QString &error);
private:
    static CDTTableExporterWriter *getWriterByName(const QString& path);
};


#endif // CDTTABLEEXPORTER_H
