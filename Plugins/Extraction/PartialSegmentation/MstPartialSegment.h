#ifndef MSTPARTIALSEGMENT_H
#define MSTPARTIALSEGMENT_H


#include <gdal_priv.h>
#include <map>
#include <vector>
#include <limits>
#include <QObject>
#include <QMap>
#include <QVector>
#include "cdtsegmentationinterface.h"

#ifndef Q_MOC_RUN
#include <stxxl/vector>
#include <stxxl/sorter>
#include <stxxl/algorithm>
#include <stxxl/bits/version.h>
#endif
class MSTMethodPrivate;
class GraphKruskal;


class MstPartialSegment
{
public:
    MstPartialSegment();
    ~MstPartialSegment();

    QVector<double> m_veclayerWeights;

    GDALDataset *m_pSrcDS;
    GDALDataset *m_pDstDS;

    QString m_strMarkfilePath;

    OGRPolygon* m_pPolygon;

    int m_nXOff;
    int m_nYOff;
    int m_nWidth;
    int m_nHeight;

    bool Start();
    bool CheckAndInit();
    bool CreateEdgeWeights();

private:
    void onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2,
                             const QVector<double> &data1, const QVector<double> &data2,
                             const QVector<double> &layerWeight, void *p);
};

#endif // MSTPARTIALSEGMENT_H
