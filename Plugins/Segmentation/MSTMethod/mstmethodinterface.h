#ifndef MSTMETHODINTERFACE_H
#define MSTMETHODINTERFACE_H

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

enum EdgeWeightMethod
{Euclid,Cosine,NDVI};




class MSTMethodInterface:public CDTSegmentationInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "MSTMethod.json")
#else
    Q_INTERFACES(CDTSegmentationInterface)
#endif // QT_VERSION >= 0x050000

    Q_PROPERTY(double Threshold READ threshold WRITE setThreshold DESIGNABLE true USER true)
    Q_PROPERTY(int Minimal_Object_Count READ minObjectCount WRITE setMinObjectCount DESIGNABLE true USER true)
    Q_PROPERTY(bool Shield_Null_Value READ shieldNulValue WRITE setShieldNulValue DESIGNABLE true USER true)
    //TODO Bigger than 0

public:
    explicit MSTMethodInterface(QObject* parent = 0);
    ~MSTMethodInterface();
    QString segmentationMethod()const;
    void startSegmentation();

    double  threshold()const;
    int minObjectCount() const;
    bool shieldNulValue() const;

    void setThreshold(double val);
    void setMinObjectCount(int val);
    void setShieldNulValue(bool val);

private:
    MSTMethodPrivate *pData;
    void *srcDS;
    void *dstDS;
    QVector<double> _layerWeights;

    bool _CheckAndInit();
    bool _CreateEdgeWeights(void *p);
    bool _ObjectMerge(GraphKruskal *&graph, void *p, unsigned num_vertices, double threshold);
    bool _EliminateSmallArea(GraphKruskal *&graph, void *p, double _minObjectSize);
    bool _GenerateFlagImage(GraphKruskal *&graph,const QMap<unsigned, unsigned> &mapNodeidObjectid);
    bool _Polygonize();

private slots:
    void onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2,
                             const QVector<double> &data1, const QVector<double> &data2,
                             const QVector<double> &layerWeight, void *p);
};

#endif // MSTMETHODINTERFACE_H
