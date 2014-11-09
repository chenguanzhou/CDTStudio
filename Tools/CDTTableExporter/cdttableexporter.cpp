#include "cdttableexporter.h"
#include <QtCore>
#include <QtSql>

class CSVWriter:public CDTTableExporterWriter
{
public:
    CSVWriter(const QString &path)
        :CDTTableExporterWriter(path)
    {
        file.setFileName(path);
        if (file.open(QFile::WriteOnly)==false) valid = false;
        out.setDevice(&file);
    }
    ~CSVWriter()
    {
        file.close();
    }

    void writeLine(QStringList record)
    {
        if (valid == true)
            out<<record.join(",")<<endl;
    }

private:
    QFile file;
    QTextStream out;
};

class TXTWriter:public CDTTableExporterWriter
{
public:
    TXTWriter(const QString &path)
        :CDTTableExporterWriter(path)
    {
        file.setFileName(path);
        if (file.open(QFile::WriteOnly)==false) valid = false;
        out.setDevice(&file);
    }
    ~TXTWriter()
    {
        file.close();
    }

    void writeLine(QStringList record)
    {
        if (valid == true)
            out<<record.join(" ")<<endl;
    }

private:
    QFile file;
    QTextStream out;
};

bool CDTTableExporter::exportSingleTable(QSqlDatabase &db, const QString &tableName, const QString &exportPath, bool isHeader, QString &error)
{
    QSqlQuery query(db);
    if (!db.tables().contains(tableName))
    {
        error = QObject::tr("Table '%1' is not exist in db").arg(tableName);
        return false;
    }
    if (query.exec(QString("select * from ")+tableName)==false)
    {
        error = QObject::tr("Fialed to select table %1").arg(tableName);
        return false;
    }

    CDTTableExporterWriter *writer = getWriterByName(exportPath);
    if (writer == NULL)
    {
        error = QObject::tr("No fit writer");
        return false;
    }

    //Header
    if (isHeader)
    {
        QStringList fields;
        for(int i=0;i<query.record().count();++i)
            fields<<query.record().fieldName(i);
        writer->writeLine(fields);
    }

    while (query.next())
    {
        QStringList record;
        for (int i=0;i<query.record().count();++i)
        {
            record<<query.value(i).toString();
        }
        writer->writeLine(record);
    }
    delete writer;
    return true;
}

bool CDTTableExporter::exportMultiTables(QSqlDatabase &db, const QStringList &tableNames, const QString &exportPath,bool isHeader,QString &error)
{
    return true;
}

CDTTableExporterWriter *CDTTableExporter::getWriterByName(const QString &path)
{
    if (path.isEmpty())
        return NULL;
    QString suffix = QFileInfo(path).completeSuffix().toLower();
    CDTTableExporterWriter *writer = NULL;
    if (suffix == "csv")
    {
        writer = new CSVWriter(path);
        if (!writer->isValid())
            return NULL;
    }
    else if (suffix == "txt")
    {
        writer = new TXTWriter(path);
        if (!writer->isValid())
            return NULL;
    }
    return writer;
}

