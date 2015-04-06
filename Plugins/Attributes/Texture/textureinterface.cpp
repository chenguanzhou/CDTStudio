#include "textureinterface.h"
#include <gdal_priv.h>
#include <QDebug>

TextureInterface::TextureInterface( QObject *parent)
    :CDTAttributesInterface(parent)
{
    _mapHaralickNames.insert("GLCM",GetGlcmHaralickName());
    _mapHaralickNames.insert("GLDV",GetGldvHaralickName());
}

TextureInterface::~TextureInterface()
{
}

QString TextureInterface::attributesType() const
{
    return tr("Texture");
}

QString TextureInterface::tableName() const
{
    return QString("Texture");
}

QStringList TextureInterface::attributesName(QString name, QString funcName)
{
    QStringList haralickNames = _mapHaralickNames[funcName];
    for (QStringList::iterator iter = haralickNames.begin();iter!=haralickNames.end();++iter)
        *iter = name + "_" + *iter;
    return haralickNames;
}

QStringList TextureInterface::GetGlcmHaralickName()
{
    return QStringList()<<"Homogeneity"<<"Contrast"<<"Dissimilarity"<<"Mean"
                          <<"StandardDeviation"<<"Entropy"<<"AngularSecondMoment"
                            <<"Correlation";
}

QStringList TextureInterface::GetGldvHaralickName()
{
    return QStringList()<<"AngularSecondMoment"<<"Entropy"<<"Mean"<<"Contrast";
}

QVector<qreal> TextureInterface::GLCM(const AttributeParamsSingleAngleBand &param) const
{

    const int SIXTEEN = 16;
    const int blockCount = (param.buffer.maxVal - param.buffer.minVal)/SIXTEEN+1;
    const double blockOffset = param.buffer.minVal;

    QPoint _offset = estimateOffset(param.buffer.angle);

    QVector<qreal> refValue;

    refValue.clear();
    refValue.resize(_mapHaralickNames["GLCM"].size());

    QVector<QPoint> pointsAnother(param.pointsVecI.size());
    std::transform(param.pointsVecI.begin(),param.pointsVecI.end(),pointsAnother.begin(),[&](const QPoint &pt)->QPoint
    { return QPoint(pt.x()+_offset.x(),pt.y()+_offset.y()); });

    QVector<double> glcm(SIXTEEN*SIXTEEN,0);
    int count = 0;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        if (pointsAnother[i].x() >=0 && pointsAnother[i].x() < param.nXSize && pointsAnother[i].y() >=0 && pointsAnother[i].y() < param.nYSize)
        {
            int level_1 = ( SRCVAL(param.buffer.buf,param.dataType,param.pointsVecI[i].y() * param.nXSize
                                   + param.pointsVecI[i].x() ) - blockOffset) / blockCount;
            int level_2 = ( SRCVAL(param.buffer.buf,param.dataType,pointsAnother[i].y() * param.nXSize
                                   + pointsAnother[i].x() ) - blockOffset) / blockCount;
            ++glcm[level_1*SIXTEEN + level_2];
            ++count;
        }
    }

    if (count == 0)
    {
        return refValue;
    }

    for (int i=0;i<glcm.size();++i)
        glcm[i] /= count;

    double mean1 = 0.0;
    double mean2 = 0.0;
    double var1  = 0.0;
    double var2  = 0.0;
    for (int j = 0; j < SIXTEEN; ++j)
    {
        for (int k = 0; k < SIXTEEN; ++k)
        {
            refValue[0] += glcm[j*SIXTEEN + k] / (1+(j-k)*(j-k));
            refValue[1] += glcm[j*SIXTEEN + k] * (j-k)*(j-k);
            refValue[2] += glcm[j*SIXTEEN + k] * sqrt(static_cast<double>((j-k)*(j-k)));
            mean1       += glcm[j*SIXTEEN + k] * j;
            mean2       += glcm[j*SIXTEEN + k] * k;
            refValue[5] += fabs(glcm[j*SIXTEEN + k])<0.00001?0:(-glcm[j*SIXTEEN + k]*log(glcm[j*SIXTEEN + k]));
            refValue[6] += glcm[j*SIXTEEN + k] * glcm[j*SIXTEEN + k];
        }
    }

    for (int j = 0; j < SIXTEEN; ++j)
    {
        for (int k = 0; k < SIXTEEN; ++k)
        {
            var1        += glcm[j*SIXTEEN + k]*(j - mean1)*(j - mean1);
            var2        += glcm[j*SIXTEEN + k]*(k - mean2)*(k - mean2);
        }
    }

    refValue[3]         = mean1;
    refValue[4]         = sqrt(var1);

    for (int j = 0; j < SIXTEEN; ++j)
    {
        for (int k = 0; k < SIXTEEN; ++k)
        {
            refValue[7] += glcm[j*SIXTEEN + k]*(j - mean1)*(k - mean2)/sqrt(var1*var2);
        }
    }

    return refValue;
}

QVector<qreal> TextureInterface::GLDV(const AttributeParamsSingleAngleBand &param) const
{
    const int SIXTEEN =16;
    const int blockCount = (param.buffer.maxVal - param.buffer.minVal)/SIXTEEN+1;
    const double blockOffset = param.buffer.minVal;

    QPoint _offset = estimateOffset(param.buffer.angle);
    QVector<qreal> refValue;

    refValue.clear();
    refValue.resize(_mapHaralickNames["GLDV"].size());

    std::vector<QPoint> pointsAnother(param.pointsVecI.size());
    std::transform(param.pointsVecI.begin(),param.pointsVecI.end(),pointsAnother.begin(),[&](const QPoint &pt)->QPoint
    { return QPoint(pt.x()+_offset.x(),pt.y()+_offset.y());   });

    std::vector<double> glcm(SIXTEEN*SIXTEEN,0);
    int count = 0;
    for (int i=0;i<param.pointsVecI.size();++i)
    {
        if (pointsAnother[i].x() >=0 && pointsAnother[i].x() < param.nXSize && pointsAnother[i].y() >=0 && pointsAnother[i].y() < param.nYSize)
        {
            int level_1 = (SRCVAL(param.buffer.buf,param.dataType,param.pointsVecI[i].y() * param.nXSize
                                  + param.pointsVecI[i].x() ) - blockOffset) / blockCount;
            int level_2 = (SRCVAL(param.buffer.buf,param.dataType,pointsAnother[i].y() *param. nXSize
                                  + pointsAnother[i].x() ) - blockOffset) / blockCount;
            ++glcm[level_1*SIXTEEN + level_2];
            ++count;
        }
    }

    if (count == 0)
    {
        return refValue;
    }
    for (unsigned int i=0;i<glcm.size();++i)
        glcm[i] /= count;

    std::vector<double> gldv(SIXTEEN,0);

    for(int i =0;i <SIXTEEN;++i)
    {
        for(int j =0;j <SIXTEEN;++j)
        {

            gldv[(i-j)>0?(i-j):(j-i)] += glcm[j*SIXTEEN + i];

        }
    }


    for(unsigned int i =0;i< gldv.size();++i)
    {
        refValue[0] +=gldv[i] * gldv[i] ;
        refValue[1] += (fabs(gldv[i])<0.00001?0:(-gldv[i]*log(gldv[i])));
        refValue[2] +=gldv[i] * i;
        refValue[3] +=gldv[i] * i * i ;
    }

    return  refValue;
}

QPoint TextureInterface::estimateOffset(int angle) const
{
    QPoint offset;
    if (angle == 0)
        offset = QPoint(0,1);
    else if ( angle == 45)
        offset = QPoint(1,1);
    else if (angle == 90)
        offset = QPoint(1,0);
    else if (angle == 135)
        offset = QPoint(-1,1);
    return offset;

}

#if QT_VERSION < 0x050000
Q_EXPORT_PLUGIN2(texture, TextureInterface)
#endif // QT_VERSION < 0x050000
