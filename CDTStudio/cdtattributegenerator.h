#ifndef CDTATTRIBUTEGENERATOR_H
#define CDTATTRIBUTEGENERATOR_H

#include "cdtbasethread.h"
#include <gdal_priv.h>
#include <ogr_api.h>
#include <ogrsf_frmts.h>
#include <QtSql>
#include "cdtattributesinterface.h"

const double PI = 3.1415926536;

class CDTAttributeGenerator : public CDTBaseThread
{
    Q_OBJECT
public:
    explicit CDTAttributeGenerator(const QString &imagePath,
                                   const QString &flagPath,
                                   const QString &shpPath,
                                   QSqlDatabase db,
                                   const QMap<QString,QStringList> &attributes,
                                   QObject *parent = 0);

    ~CDTAttributeGenerator();

    bool isValid(){return _isValid;}
    QString errorInfo()const {return _errorInfo;}
    void run();
signals:

public slots:

private:
    bool            _isValid;
    GDALDataset*    _poImageDS;
    GDALDataset*    _poFlagDS;
    OGRDataSource*  _poGeometryDS;
    QMap<QString,QStringList> _attributes;
    QMap<QString,CDTAttributesInterface*> _plugins;
    QMap<int,OGRPolygon *> _geometryObjects;

    int             _bandCount;
    int             _width;
    int             _height;
    GDALDataType    _dataType;  //GDALDatatype
    int             _dataSize;  //Data size per pixel
    int             _objectCount;

    QString         _errorInfo;

    bool readGeometry();
    bool initAttributeTable();
    bool computeAttributes(QMap<QString, QList<QVector<double> > > &attributesValues, QMap<QString, QStringList> &attributesFieldNames);
    bool addAttributesToTables(QMap<QString, QList<QVector<double> > > &attributesValues, QMap<QString, QStringList> &attributesFieldNames);
};

class ObjectInfo
{
public:
    ObjectInfo(int xx=0,int yy=0,int id=0):x(xx),y(yy),ObjectID(id){}
    int x,y,ObjectID;
};

struct ObjectInfoComparator
{
    bool operator () (const ObjectInfo& a, const ObjectInfo& b) const
    {
        return a.ObjectID < b.ObjectID;
    }
    ObjectInfo min_value() const
    {
        return ObjectInfo(0,0,std::numeric_limits<int>::min());
    }
    ObjectInfo max_value() const
    {
        return ObjectInfo(0,0,std::numeric_limits<int>::max());
    }
};

#endif // CDTATTRIBUTEGENERATOR_H
