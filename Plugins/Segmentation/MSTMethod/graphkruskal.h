#ifndef GRAPHKRUSKAL_H
#define GRAPHKRUSKAL_H

#include <vector>
#include <map>
#include <cmath>
#include <gdal_priv.h>
#include <QTemporaryFile>
#include <QMap>

class GraphElement {
public:
    GraphElement():rank(0),p(0),size(1),sw(0){}

    unsigned rank;
    unsigned p; //Root node
    unsigned size;
    float sw;//Sum of weights
};


class GraphKruskal
{
public:

    GraphKruskal(unsigned elements);
    ~GraphKruskal();

    unsigned find(unsigned x);
    bool joinPredicate_sw(unsigned reg1, unsigned reg2, float th,float edgeWeight,int nPredict);//Is merge or not
    unsigned join_band_sw(unsigned x, unsigned y, float edgeWeight);//Merge and return the root node
    unsigned size(unsigned x) { return elts[x].size; }


    void GetMapNodeidObjectid(GDALRasterBand*& poMaskBand, QMap<unsigned, unsigned> &mapRootidObjectid);

public:
    int num;
    int elementCount;
    GraphElement* elts;
    QTemporaryFile file;
};


#endif // GRAPHKRUSKAL_H
