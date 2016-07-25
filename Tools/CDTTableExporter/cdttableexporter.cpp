#include "cdttableexporter.h"
#include <QtCore>
#include <QtSql>
#include <QMessageBox>
#ifdef Q_OS_WIN
#include <QAxObject>
#endif

class CSVWriter:public CDTSingleTableExporterWriter
{
public:
    CSVWriter(const QString &path)
        :CDTSingleTableExporterWriter(path)
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

class TXTWriter:public CDTSingleTableExporterWriter
{
public:
    TXTWriter(const QString &path)
        :CDTSingleTableExporterWriter(path)
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

#ifdef Q_OS_WIN
class ExcelWriter:public CDTSingleTableExporterWriter
{
public:
    ExcelWriter(const QString &path)
        :CDTSingleTableExporterWriter(path),rowCount(0),colCount(0)
    {
        excel = new QAxObject( "Excel.Application", 0 );
        QAxObject*workbooks = excel->querySubObject( "Workbooks" );
        workbook = workbooks->querySubObject( "Add");
        QAxObject* sheets = workbook->querySubObject( "Worksheets" );
        currentSheet = sheets->querySubObject( "Item( int )", 1 );

        QFile file(path);
        if (file.exists())
        {
            if (file.remove()==false)
            {
                valid = false;
            }
        }
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
                              QDir::toNativeSeparators(filePath),QVariant(),QVariant(),QVariant(),QVariant(),QVariant(),QVariant(),2);
        workbook->dynamicCall("Close()");
        excel->dynamicCall("Quit()");
    }

    void writeLine(QStringList record)
    {
        if (colCount==0)
            colCount = record.count();
        QVariantList line;
        foreach (QString val, record) {
            line<<val;
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

bool CDTTableExporter::exportSingleTable(QSqlDatabase &db, const QString &tableName, const QString &exportPath,bool isHeader, QString &error)
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

    CDTSingleTableExporterWriter *writer = getWriterByNameSingle(exportPath);
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
#ifdef Q_OS_WIN
    QAxObject* excel = new QAxObject( "Excel.Application", 0 );
    QAxObject* workbooks = excel->querySubObject( "Workbooks" );
    QAxObject* workbook = workbooks->querySubObject( "Add");
    QAxObject* sheets = workbook->querySubObject( "Worksheets" );

    QFile file(exportPath);
    if (file.exists())
    {
        if (file.remove()==false)
        {
            error = QObject::tr("Delete file %1 failed!").arg(exportPath);
            return false;
        }
    }

    for (int i=0;i<tableNames.count()-1;++i)
        sheets->dynamicCall("Add()");

    int count = sheets->property("Count").toInt();
    if (count != tableNames.count())
    {
        error = "Add sheet failed!";
        return false;
    }

    QSqlQuery query(db);
    for (int i=0;i<count;++i)
    {
        QString tableName = tableNames[i];
        QAxObject* currentSheet = sheets->querySubObject( "Item( int )", i+1 );
        currentSheet->setProperty("Name",tableName);

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

        int rowCount = 0,colCount = 0;

        QVariantList data;
        if (isHeader)
        {
            QVariantList line;
            for(int i=0;i<query.record().count();++i)
                line<<query.record().fieldName(i);
            data.append(QVariant(line));
            ++rowCount;
        }

        while (query.next())
        {
            if (colCount==0)
                colCount = query.record().count();
            QVariantList line;
            for (int i=0;i<query.record().count();++i)
            {
                line<<query.value(i).toString();
            }
            data.append(QVariant(line));
            ++rowCount;
        }

        QAxObject* items = currentSheet->querySubObject( "Cells" );
        QAxObject* range = currentSheet->querySubObject( "Range(QVariant, QVariant)",
                                                  items->dynamicCall("Item(QVariant, QVariant)", 1,1),
                                                  items->dynamicCall("Item(QVariant, QVariant)", rowCount,colCount));
        range->setProperty("Value",data);
    }

    workbook->dynamicCall("SaveAs(QVariant, QVariant, QVariant, QVariant, QVariant, QVariant, XlSaveAsAccessMode, QVariant, QVariant, QVariant, QVariant, QVariant)",
                          QDir::toNativeSeparators(exportPath),QVariant(),QVariant(),QVariant(),QVariant(),QVariant(),QVariant(),2);
    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");
    delete excel;
    return true;
#endif
    QMessageBox::critical(NULL,QObject::tr("Error"),QObject::tr("Export multiple tables are not supported in this plateform!"));
    return false;
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

QString CDTTableExporter::getMiltiExporterFilters()
{
#ifdef Q_OS_WIN
    return QObject::tr("Microsoft Excel Files(*.xlsx *.xls)");
#else
    return QString();
#endif
}

CDTSingleTableExporterWriter *CDTTableExporter::getWriterByNameSingle(const QString &path)
{
    if (path.isEmpty())
        return NULL;
    QString suffix = QFileInfo(path).completeSuffix().toLower();
    CDTSingleTableExporterWriter *writer = NULL;
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

