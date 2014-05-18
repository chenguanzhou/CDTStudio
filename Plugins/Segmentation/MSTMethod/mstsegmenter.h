#ifndef MSTSEGMENTER_H
#define MSTSEGMENTER_H

#include <QObject>
#include "cdtbasethread.h"
#include <map>
#include <vector>
#include <limits>
#include <QMap>
#include <QVector>

class GraphKruskal;

enum EdgeWeightMethod
{Euclid,Cosine,NDVI};

class edge
{
public:
    edge(){}
    edge(unsigned nodeID1,unsigned nodeID2,float weight):_weight(weight),_nodeID1(nodeID1),_nodeID2(nodeID2){}
    bool operator<(const edge& other) const
    {
        return _weight<other._weight;
    }
    float GetWeight()const{return _weight;}
    unsigned GetNode1()const{return _nodeID1;}
    unsigned GetNode2()const{return _nodeID2;}
private:
    float _weight;
    unsigned _nodeID1;
    unsigned _nodeID2;
};

struct edge_comparator
{
    bool operator () (const edge& a, const edge& b) const
    {
        return a.GetWeight() < b.GetWeight();
    }
    edge min_value() const
    {
        return edge(0,0,std::numeric_limits<float>::min());
    }
    edge max_value() const
    {
        return edge(0,0,std::numeric_limits<float>::max());
    }
};

class GraphVertex
{
public:
    GraphVertex(unsigned xx=0,unsigned yy=0):x(xx),y(yy){}
    unsigned x,y;
};

class GraphEdge
{
public:
    float weight;
};

class MSTSegmenter:public CDTBaseThread
{
    Q_OBJECT
public:


    MSTSegmenter(const QString &inputImagePath,
                 const QString &outputImagePath,
                 const QString &shapefilePath,
                 int MergeRule ,
                 double threshold,
                 int minObjectSize,
                 bool shield,
                 QVector<double> layerWeights=QVector<double>(),
                 QObject *parent = 0
                 );

    ~MSTSegmenter();

    void run();

signals:
//    void currentProgressChanged(QString);
//    void progressBarValueChanged(int);//0-100
//    void progressBarSizeChanged(int,int);
//    void showNormalMessage(QString);
//    void showWarningMessage(QString);
//    void aborted();
//    void done();

private:
    QString             _inputImagePath;
    QString             _outputImagePath;
    QString             _shapefilePath;
    int                 _mergeRule;             //The Merge Rule Of the Algorithm
    double              _threshold;             //The Threshold of Segmentation
    int                 _minObjectSize;         //The minimal Object Size in Output Image
    bool                shield_0_255;
    QVector<double>     _layerWeights;

//    GDALDataset* poSrcDS;
//    GDALDataset* poDstDS;

    void *srcDS;
    void *dstDS;

    bool _CheckAndInit();
    bool _CreateEdgeWeights(void *p);
    bool _ObjectMerge(GraphKruskal *&graph, void *p, unsigned num_vertices, double threshold);
    bool _EliminateSmallArea(GraphKruskal *&graph, void *p, double _minObjectSize);
    bool _GenerateFlagImage(GraphKruskal *&graph,const QMap<unsigned, unsigned> &mapNodeidObjectid);

signals:
   // void computeEdgeWeight(unsigned , unsigned , QVector<double> , QVector<double>, QVector<double>, void *);
private slots:
    void onComputeEdgeWeight(unsigned nodeID1, unsigned nodeID2,
                             const QVector<double> &data1, const QVector<double> &data2,
                             const QVector<double> &layerWeight, void *p);
};


#endif // MSTSEGMENTER_H
