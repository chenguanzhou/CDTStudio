#include "cdttableexporter.h"
#include <QtCore>
#include <QtSql>
#include <QMessageBox>
#ifdef Q_OS_WIN
#include <QAxObject>
#endif

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

    bool isHeader() const
    {
        return (QMessageBox::information(NULL,
                                        QObject::tr("Write headers?"),
                                        QObject::tr("Write first line as headers?"),
                                        QMessageBox::Ok|QMessageBox::No)==QMessageBox::Ok);
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

    bool isHeader() const
    {
        return (QMessageBox::information(NULL,
                                        QObject::tr("Write headers?"),
                                        QObject::tr("Write first line as headers?"),
                                        QMessageBox::Ok|QMessageBox::No)==QMessageBox::Ok);
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

#ifdef Q_OS_WIN
class ExcelWriter:public CDTTableExporterWriter
{
public:
    ExcelWriter(const QString &path)
        :CDTTableExporterWriter(path),rowCount(0),colCount(0)
    {
        excel = new QAxObject( "Excel.Application", 0 );
        QAxObject*workbooks = excel->querySubObject( "Workbooks" );
        workbook = workbooks->querySubObject( "Add");
        QAxObject* sheets = workbook->querySubObject( "Worksheets" );
        currentSheet = sheets->querySubObject( "Item( int )", 1 );
    }
    ~ExcelWriter()
    {
        QAxObject* items = currentSheet->querySubObject( "Cells" );
        QAxObject* range = currentSheet->querySubObject( "Range(QVariant, QVariant)",
                                                  items->dynamicCall("Item(QVariant, QVariant)", 1,1),
                                                  items->dynamicCall("Item(QVariant, QVariant)", rowCount,colCount));
        qDebug()<<rowCount<<colCount;
        range->setProperty("Value",data);
        workbook->dynamicCall("SaveAs(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, XlSaveAsAccessMode, QVariant, QVariant, QVariant, QVariant, QVariant)",
                              filePath.toLocal8Bit().constData());
        workbook->dynamicCall("Close()");
        excel->dynamicCall("Quit()");
    }

    void writeLine(QStringList record)
    {
        if (colCount==0)
            colCount = record.count();
        QVariantList line;
        foreach (QString val, record) {
            line<<val.toDouble();
        }
        data.append(QVariant(line));
        ++rowCount;
    }

private:
    QAxObject* excel;
    QAxObject* workbook;
    QAxObject* currentSheet;
    QVariantList data;
    int rowCount;
    int colCount;
};
#endif

bool CDTTableExporter::exportSingleTable(QSqlDatabase &db, const QString &tableName, const QString &exportPath, QString &error)
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

    CDTTableExporterWriter *writer = getWriterByNameSingle(exportPath);
    if (writer == NULL)
    {
        error = QObject::tr("No fit writer");
        return false;
    }

    //Header
    if (writer->isHeader())
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

QString CDTTableExporter::getSingleExporterFilters()
{
    QStringList list;

#ifdef Q_OS_WIN
    list<<QObject::tr("Microsoft Excel Files(*.xlsx *.xls)");
#endif
    list<<QObject::tr("CSV Files(*.csv)");
    list<<QObject::tr("Text Files(*.txt)");
    return list.join(";;");
}

CDTTableExporterWriter *CDTTableExporter::getWriterByNameSingle(const QString &path)
{
    if (path.isEmpty())
        return NULL;
    QString suffix = QFileInfo(path).completeSuffix().toLower();
    CDTTableExporterWriter *writer = NULL;
#ifdef Q_OS_WIN
    if (suffix == "xlsx" || suffix == "xls")
    {
        writer = new ExcelWriter(path);
        if (!writer->isValid())
            return NULL;
    }
#endif
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

