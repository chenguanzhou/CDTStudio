#include "cdtattributegenerator.h"
#include <numeric>
#include <opencv2/opencv.hpp>

#ifndef Q_MOC_RUN
#include <stxxl/sorter>
#endif

extern QList<CDTAttributesInterface *>     attributesPlugins;
typedef stxxl::sorter<ObjectSorterElement,ObjectSorterElementComparator,sizeof(ObjectSorterElement)*1024*1024> ObjectSorter;
typedef stxxl::VECTOR_GENERATOR<ObjectInfo>::result ObjectVector;

CDTAttributeGenerator::CDTAttributeGenerator(
        const QString &imagePath,
        const QString &flagPath,
        const QString &shpPath,
        QSqlDatabase db,
        const QMap<QString, QStringList> &attributes,
        QObject *parent)
    : CDTBaseThread(parent), _attributes(attributes)
{
    GDALAllRegister();
    OGRRegisterAll();
    CPLSetConfigOption("GDAL_FILENAME_IS_UTF8","YES");

    _poImageDS = (GDALDataset*)GDALOpen(imagePath.toUtf8().constData(),GA_ReadOnly);
    if (_poImageDS == NULL)
    {
        _errorInfo = tr("Open Image ")+imagePath+tr(" Failed!");
        _isValid = false;
        return;
    }

    _poFlagDS = (GDALDataset*)GDALOpen(flagPath.toUtf8().constData(),GA_ReadOnly);
    if (_poFlagDS == NULL)
    {
        _errorInfo = tr("Open Flag Image ")+flagPath+tr(" Failed!");
        _isValid = false;
        return;
    }


    _poGeometryDS =  (GDALDataset*)GDALOpenEx( shpPath.toUtf8().constData(),GDAL_OF_VECTOR,NULL,NULL,NULL);
    if (_poGeometryDS == NULL)
    {
        _errorInfo = tr("Open Vector File ")+shpPath+tr(" Failed!");
        _isValid = false;
        return ;
    }

    if (db.isValid() == false)
    {
        _errorInfo = tr("Open Database Failed!");
        _isValid = false;
        return ;
    }
//    _db = db;

    if (db.driverName()=="QSQLITE")
    {
        QSqlQuery query("PRAGMA synchronous = OFF; ",db);
        if (!query.isActive())
        {
            _errorInfo = query.lastError().text();
            _isValid = false;
            return ;
        }
    }

    foreach (CDTAttributesInterface* plugin, attributesPlugins) {
        _plugins.insert(plugin->attributesType(),plugin);
    }

    _width      = _poImageDS->GetRasterXSize();
    _height     = _poImageDS->GetRasterYSize();
    _bandCount  = _poImageDS->GetRasterCount();

    _dataType   = _poImageDS->GetRasterBand(1)->GetRasterDataType();
    _dataSize   = GDALGetDataTypeSize(_dataType)/8;
    _isValid    = true;
}

CDTAttributeGenerator::~CDTAttributeGenerator()
{
    GDALClose(_poImageDS);
    GDALClose(_poFlagDS);
    GDALClose( _poGeometryDS );
}

void CDTAttributeGenerator::run()
{
    QTime t;
    t.start();
    if (readGeometry()==false)
        return;

    ObjectVector objectInfoVector;
    if (initAttributeTable(&objectInfoVector)==false)
        return;

    qDebug()<<"initAttributeTable cost: "<<t.elapsed()<<"ms";
    QMap<QString,QList<QVector<double> > > attributesValues;
    QMap<QString,QStringList> attributesFieldNames;
    if (computeAttributes(&objectInfoVector,attributesValues,attributesFieldNames)==false)
        return;

    addAttributesToTables(attributesValues,attributesFieldNames);
    qDebug()<<"Attribute generation cost: "<<t.elapsed()<<"ms";
}

bool CDTAttributeGenerator::readGeometry()
{
    OGRLayer *layer = _poGeometryDS->GetLayer(0);
    if (layer == NULL)
    {
        emit showWarningMessage(tr("Open Shapefile Layer failed!"));
        return false;
    }

    OGRFeatureDefn* defn = layer->GetLayerDefn();
    if (defn == NULL)
    {
        emit showWarningMessage(tr("Read Shapefile Defn Failed!"));
        return false;
    }

    const int gridCodeID = defn->GetFieldIndex("GridCode");
    _geometryObjects.clear();

    emit currentProgressChanged(tr("Load Geometry Information"));
    int barSize = layer->GetFeatureCount();
    emit progressBarSizeChanged(0,barSize);
    int progressGap = barSize/100;
    int index = 0;
    OGRFeature *feature = layer->GetNextFeature();
    while (feature != NULL)
    {
        int objectID = feature->GetFieldAsInteger(gridCodeID);

        OGRGeometry *geometry = feature->GetGeometryRef();
        if (geometry->getGeometryType() == wkbPolygon)
        {
            _geometryObjects[objectID] = (OGRPolygon *)geometry;
        }

        if (index%progressGap==0)
            emit progressBarValueChanged(index);
        feature = layer->GetNextFeature();
        ++index;
    }
    emit progressBarValueChanged(barSize);
    return true;
}

bool CDTAttributeGenerator::initAttributeTable(void *p)
{
    ObjectVector *objVector = static_cast<ObjectVector *>(p);

    emit currentProgressChanged(tr("Initializing Object Information"));
    int barSize = _height;
    emit progressBarSizeChanged(0,barSize);
    int progressGap = barSize/100;

    ObjectSorter sorter(ObjectSorterElementComparator(),500*1024*1024);
    GDALRasterBand* poFlagBand = _poFlagDS->GetRasterBand(1);
    GDALRasterBand* poMaskBand = poFlagBand->GetMaskBand();
    int* buffer = new int[_width];
    if (poMaskBand == NULL)//No mask
    {
        for(int i=0;i<_height;++i)
        {
            poFlagBand->RasterIO(GF_Read,0,i,_width,1,buffer,_width,1,GDT_Int32,0,0);
            for(int j=0;j<_width;++j)
                sorter.push(ObjectSorterElement(j,i,buffer[j]));

            if (i%progressGap==0)
                emit progressBarValueChanged(i);
        }
    }
    else//Mask exists
    {
        uchar* mask = new uchar[_width];
        for(int i=0;i<_height;++i)
        {
            poFlagBand->RasterIO(GF_Read,0,i,_width,1,buffer,_width,1,GDT_Int32,0,0);
            poMaskBand->RasterIO(GF_Read,0,i,_width,1,mask,_width,1,GDT_Byte,0,0);
            for(int j=0;j<_width;++j)
            {
                if (mask[j]==0)
                    continue;
                sorter.push(ObjectSorterElement(j,i,buffer[j]));
            }
            if (i%progressGap==0)
                emit progressBarValueChanged(i);
        }
    }
    delete[] buffer;
    progressBarValueChanged(barSize);

    emit currentProgressChanged(tr("Sort object information by ID"));
    emit progressBarValueChanged(0);

    sorter.sort();
    emit progressBarValueChanged(barSize);

    int currentID = 0;
    std::vector<QPoint> objectPoints;
    int xmin = std::numeric_limits<int>::max();
    int xmax = std::numeric_limits<int>::min();
    int ymin = std::numeric_limits<int>::max();
    int ymax = std::numeric_limits<int>::min();

    emit currentProgressChanged(tr("Merge Object Information"));
    barSize = sorter.size();
    emit progressBarSizeChanged(0,barSize);
    progressGap = barSize/100;
    int index = 0;

    while(!sorter.empty())
    {
        if (sorter->ObjectID == currentID)//continue accumulate
        {
            xmin = xmin>sorter->x?sorter->x:xmin;
            xmax = xmax<sorter->x?sorter->x:xmax;
            ymin = ymin>sorter->y?sorter->y:ymin;
            ymax = ymax<sorter->y?sorter->y:ymax;
            objectPoints.push_back(QPoint(sorter->x,sorter->y));
        }
        else//insert into sqlite
        {
            objVector->push_back(ObjectInfo(
                currentID,objectPoints,xmin,xmax,ymin,ymax));

            currentID = sorter->ObjectID;
            xmin = xmax = sorter->x;
            ymin = ymax = sorter->y;
            objectPoints.clear();
            objectPoints.push_back(QPoint(sorter->x,sorter->y));
        }
        ++sorter;
        if (index%progressGap==0)
            emit progressBarValueChanged(index);
        ++index;
    }

    objVector->push_back(ObjectInfo(
        currentID,objectPoints,xmin,xmax,ymin,ymax));
    emit progressBarValueChanged(barSize);
    _objectCount = currentID+1;


    return true;
}

bool CDTAttributeGenerator::computeAttributes(
        void *p,
        QMap<QString,QList<QVector<double> > > &attributesValues,
        QMap<QString,QStringList> &attributesFieldNames)
{
    attributesValues.clear();
    ObjectVector *objVector = static_cast<ObjectVector *>(p);

    double adfGeoTransform[6];
    _poImageDS->GetGeoTransform(adfGeoTransform);

    emit currentProgressChanged(tr("Generating Attributes"));
    int barSize = _objectCount;
    emit progressBarSizeChanged(0,barSize);
    int progressGap = barSize/100;
    int index = 0;

    QVector<double> minBandVal(_bandCount),maxBandVal(_bandCount);
    for (int k=0;k<_bandCount;++k)
    {
        double dMinMax[2];
        _poImageDS->GetRasterBand(k+1)->ComputeRasterMinMax(FALSE,dMinMax);
        minBandVal[k] = dMinMax[0];
        maxBandVal[k] = dMinMax[1];
    }

    QMap<QString,int> timer;
    clock_t time_start =clock();


    for (ObjectVector::const_iterator it = objVector->begin(); it != objVector->end(); ++it)
    {
//    for (ObjectVector::bufreader_type it(*objVector); !it.empty(); ++it)
//    {
        int objectID = it->ObjectID;
        int pixelCount = it->ObjectPoints.size();
        int nXOff = it->x_min;
        int nYOff = it->y_min;
        int nXSize = it->x_max - nXOff + 1;
        int nYSize = it->y_max - nYOff + 1;

        QVector<QPoint> pointsVecI;
        QVector<QPointF> pointsVecF;
        QVector<QPointF> rotatedPointsVec(pixelCount);

        std::for_each(it->ObjectPoints.begin(),it->ObjectPoints.end(),[&](const QPoint &pt){
            pointsVecI.push_back(pt- QPoint(nXOff,nYOff));
            qreal x = adfGeoTransform[0] + adfGeoTransform[1]*pt.x() + adfGeoTransform[2]*pt.y();
            qreal y = adfGeoTransform[3] + adfGeoTransform[4]*pt.x() + adfGeoTransform[5]*pt.y();
            pointsVecF.push_back(QPointF(x,y));
        });

        // Compute Area and Border_length
        OGRPolygon*     polygon         = _geometryObjects[objectID];
        OGRLinearRing*  linearRing      = polygon->getExteriorRing();
        double          area            = polygon->get_Area();
        double          border_lenghth  = linearRing->get_Length ();
        int             ringPointCount  = linearRing->getNumPoints();
        OGRRawPoint*    ringPoints      = new OGRRawPoint[ringPointCount];

        linearRing->getPoints(ringPoints);
        QVector<QPointF> ringPointsVec(ringPointCount);
        try{
        std::transform(ringPoints,ringPoints+ringPointCount,ringPointsVec.begin(),[&](const OGRRawPoint& ptRaw)->QPointF
        { return QPointF(ptRaw.x,ptRaw.y); });
        delete []ringPoints;
        }
        catch (const std::exception &e){
            qDebug()<<e.what();
        }

        // Compute minimal bounding rectangle
        cv::Mat pointsMat(2,pixelCount,CV_64FC1);
        cv::Mat ringRotatedPointsMat(2,ringPointCount,CV_64FC1);
        cv::Mat eValuesMat;
        cv::Mat eVectorsMat;
        double meanX = 0.;
        double meanY = 0.;
        for (int i=0;i<pixelCount;++i)
        {
            pointsMat.at<double>(0,i) = pointsVecF[i].x();
            pointsMat.at<double>(1,i) = pointsVecF[i].y();

            meanX += pointsVecF[i].x();
            meanY += pointsVecF[i].y();
        }

        meanX /= pixelCount;
        meanY /= pixelCount;

        for (int i=0;i<ringPointCount;++i)
        {
            ringRotatedPointsMat.at<double>(0,i) = ringPointsVec[i].x();
            ringRotatedPointsMat.at<double>(1,i) = ringPointsVec[i].y();
        }

        cv::Mat coVarMat = pointsMat*pointsMat.t();
        cv::eigen(coVarMat, eValuesMat, eVectorsMat);

        pointsMat               = eVectorsMat * pointsMat;
        ringRotatedPointsMat    = eVectorsMat * ringRotatedPointsMat;

        for (int i=0;i<ringPointCount;++i)
        {
            ringPointsVec[i].setX (ringRotatedPointsMat.at<double>(0,i))  ;
            ringPointsVec[i].setY (ringRotatedPointsMat.at<double>(1,i))  ;
        }

        double minX = std::numeric_limits<double>::max();
        double minY = std::numeric_limits<double>::max();
        double maxX = std::numeric_limits<double>::min();
        double maxY = std::numeric_limits<double>::min();

        for (int i=0;i<pixelCount;++i)
        {
            double x = pointsMat.at<double>(0,i);
            double y = pointsMat.at<double>(1,i);
            rotatedPointsVec[i] = QPointF(x,y);
            if (x<minX) minX = x;
            if (x>maxX) maxX = x;
            if (y<minY) minY = y;
            if (y>maxY) maxY = y;
        }

        double longSideOfMBR  = maxX-minX;
        double shortSideOfMBR = maxY-minY;
        double scale = sqrt(area/(PI*longSideOfMBR*shortSideOfMBR));
        double majorSemiAxesOfAE = longSideOfMBR  * scale;
        double minorSemiAxesOfAE = shortSideOfMBR * scale;

        QPointF rotated_center(0,0);
//        for(int i=0;i<rotatedPointsVec.size();++i)
//        {
//            rotated_center+=rotatedPointsVec[i];
//        }
        rotated_center = std::accumulate(rotatedPointsVec.begin(),rotatedPointsVec.end(),QPointF(0.,0.));
        rotated_center /= rotatedPointsVec.size();

        //buffer
        QVector<uchar*> buffer(_bandCount);
        for (int k=0;k<_bandCount;++k)
        {
            buffer[k] = new uchar[nXSize*nYSize*_dataSize];
            GDALRasterBand *poBand =  _poImageDS->GetRasterBand(k+1);
            poBand->RasterIO(GF_Read,nXOff,nYOff,nXSize,nYSize,buffer[k],nXSize,nYSize,_dataType,0,0);
        }

        foreach (CDTAttributesInterface* plugin, _plugins) {
            QString tableName = plugin->tableName();
            QVector<qreal> attributesValue;

            const QMetaObject *metaObject = plugin->metaObject();

            QStringList attributeNames = _attributes.value(plugin->attributesType());
            foreach (QString attributeName, attributeNames)
            {
                clock_t t1 = clock();
                if (attributeName.contains("_angle"))//band&&angle
                {
                    int posBand = attributeName.indexOf("_band");
                    int posAngle = attributeName.indexOf("_angle");
                    QString funName = attributeName.left(posBand);
                    int bandID = attributeName.mid(posBand+5,posAngle-posBand-5).toInt();
                    int angle = attributeName.mid(posAngle+6).toInt();
//                    for(int i=0;i <4;++i)
//                    {
                        QVector<qreal> resultVal;
                        TextureParam textureparam;
                        textureparam.angle =angle;
                        textureparam.buf =buffer[bandID-1];
                        textureparam.minVal = minBandVal[bandID-1];
                        textureparam.maxVal = maxBandVal[bandID-1];
                        AttributeParamsSingleAngleBand params(
                                    pointsVecI,textureparam,_dataType,nXSize,nYSize,pointsVecF,
                                rotatedPointsVec,ringPointsVec,area,border_lenghth,
                                longSideOfMBR,shortSideOfMBR,majorSemiAxesOfAE,
                                minorSemiAxesOfAE,rotated_center);
                        metaObject->invokeMethod(
                                    plugin,funName.toUtf8().constData(),Qt::DirectConnection,
                                    Q_RETURN_ARG(QVector<qreal>,resultVal),
                                    Q_ARG(AttributeParamsSingleAngleBand,params));

                        for(int i=0;i <resultVal.size();++i)
                        {
                            attributesValue.push_back(resultVal[i]);
                        }
//                    }
                    if (index==0)
                        attributesFieldNames[tableName].append(plugin->attributesName(attributeName,funName));

                }
                else if (attributeName.contains("_band"))//single band
                {
                    int pos = attributeName.indexOf("_band");
                    QString funcName = attributeName.left(pos);
                    int bandID = attributeName.mid(pos+5).toInt();
                    qreal resultVal=0;
                    AttributeParamsSingleBand params(
                                pointsVecI,buffer[bandID-1],_dataType,nXSize,nYSize,pointsVecF,
                            rotatedPointsVec,ringPointsVec,area,border_lenghth,
                            longSideOfMBR,shortSideOfMBR,majorSemiAxesOfAE,
                            minorSemiAxesOfAE,rotated_center);
                    metaObject->invokeMethod(
                                plugin,funcName.toUtf8().constData(),Qt::DirectConnection,
                                Q_RETURN_ARG(qreal, resultVal ),
                                Q_ARG(AttributeParamsSingleBand, params));

                    attributesValue.append(resultVal);
                    if (index==0)
                        attributesFieldNames[tableName].append(attributeName);

                }
                else//all band
                {
                    QString funcName = attributeName;
                    qreal resultVal=0;
                    AttributeParamsMultiBand params(
                                pointsVecI,buffer,_dataType,nXSize,nYSize,pointsVecF,
                                rotatedPointsVec,ringPointsVec,area,border_lenghth,
                                longSideOfMBR,shortSideOfMBR,majorSemiAxesOfAE,
                                minorSemiAxesOfAE,rotated_center);
                    metaObject->invokeMethod(
                                plugin,funcName.toUtf8().constData(),Qt::DirectConnection,
                                Q_RETURN_ARG(qreal, resultVal ),
                                Q_ARG(AttributeParamsMultiBand, params));
                    attributesValue.append(resultVal);
                    //                    if (!attributesFieldNames[tableName].contains(attributeName))
                    if (index==0)
                        attributesFieldNames[tableName].append(attributeName);
                }

                timer[attributeName] += (clock()-t1);
            }
            attributesValues[tableName].push_back(attributesValue);
        }

        for (int k=0;k<_bandCount;++k)
            delete [](buffer[k]);

        if (index%progressGap==0)
            emit progressBarValueChanged(index);        
        ++index;
    }

    qDebug()<<timer;
    qDebug()<<"cost: "<< (clock()-time_start)/(double)CLOCKS_PER_SEC<<"s";

    emit progressBarValueChanged(barSize);
//    db.commit();

    return true;
}

bool CDTAttributeGenerator::addAttributesToTables(QMap<QString,QList<QVector<double> > > &attributesValues, QMap<QString, QStringList> &attributesFieldNames)
{
    QSqlDatabase db = QSqlDatabase::database("attribute");
    QSqlQuery query(db);
    foreach (QString tableName, attributesValues.keys()) {        
        QList<QVector<double> > datas = attributesValues.value(tableName);
        if (query.exec(QString("drop table if exists ") + tableName)==false)
        {
            emit showWarningMessage(query.lastError().text());
            return false;
        }

        QString sqlCreate(QString("create table ")+tableName+"(ObjectID int");
        QString sqlInsert(QString("insert into ")+tableName+ " values(?");
        foreach (QString attributeFieldName, attributesFieldNames.value(tableName)) {
            sqlCreate += QString(",") + attributeFieldName + " double";
            sqlInsert += ",?";
        }
        sqlCreate += ")";
        sqlInsert += ")";

        if (query.exec(sqlCreate)==false)
        {
            emit showWarningMessage(query.lastError().text());
            return false;
        }

        db.transaction();
        if (query.prepare(sqlInsert)==false)
        {
            emit showWarningMessage(query.lastError().text());
            return false;
        }

        emit currentProgressChanged(tr("Write ")+tableName+tr(" Attributes into database"));
        int barSize = datas.size();
        emit progressBarSizeChanged(0,barSize);
        int progressGap = barSize/100;
        int index = 0;
        foreach (QVector<double> data, datas) {
            query.bindValue(0,index);
            for (int k=0;k<data.size();++k)
                query.bindValue(1+k,data[k]);
            query.exec();
            if (index%progressGap==0)
                emit progressBarValueChanged(index);
            ++index;
        }

        emit progressBarValueChanged(barSize);


        db.commit();
    }



    return true;
}

//BUG: rollback
