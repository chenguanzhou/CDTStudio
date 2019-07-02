#ifndef MSTPARTIALSEGMENT_H
#define MSTPARTIALSEGMENT_H


#include <gdal_priv.h>
#include <map>
#include <vector>
#include <limits>
#include <QObject>
#include <QMap>
#include <QVector>

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

    void* m_pSrcDS;
    void* m_pPolygon;
    QString m_strMarkfilePath;
    QString m_strShapefilePath;


    bool Start();
    bool CheckAndInit();
    bool CreateEdgeWeights(void *p);
    bool ObjectMerge(GraphKruskal *&graph, void *p, int num_vertices, float threshold);
    bool EliminateSmallArea(GraphKruskal *&graph, void *p, double _minObjectSize);
    bool GenerateFlagImage(GraphKruskal *&graph, const QMap<unsigned, unsigned> &mapRootidObjectid);
    bool Polygonize();

private:
    MSTMethodPrivate *pData;
    void *m_pDstDS;

    int m_nXOff;
    int m_nYOff;
    int m_nWidth;
    int m_nHeight;

    QVector<double> m_layerWeights;

    void onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2,
                             const QVector<double> &data1, const QVector<double> &data2,
                             const QVector<double> &layerWeight, void *p);
};

#endif // MSTPARTIALSEGMENT_H
