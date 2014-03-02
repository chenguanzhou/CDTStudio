#ifndef GRAPHKRUSKAL_H
#define GRAPHKRUSKAL_H

#include <vector>
#include <map>
#include <cmath>
#include <gdal_priv.h>
#include <QTemporaryFile>

class GraphElement {
public:
    GraphElement():rank(0),p(0),size(1),sw(0){}

    unsigned rank;
    unsigned p; //根节点
    unsigned size;
    float sw;//该区域边权值之和
};


class GraphKruskal
{
public:

    GraphKruskal(unsigned elements);
    ~GraphKruskal();

    unsigned find(unsigned x);
    bool joinPredicate_sw(unsigned reg1, unsigned reg2, float th,float edgeWeight,int nPredict);//5基于统计学习理论，控制区域边权和，合并时共用join_band_sw函数
    unsigned join_band_sw(unsigned x, unsigned y, float edgeWeight);//合并时统计边权和，合并并返回根节点
    unsigned size(unsigned x) { return elts[x].size; }


    void GetMapNodeidObjectid(GDALRasterBand*& poMaskBand, std::map<unsigned, unsigned> &mapRootidObjectid);

public:
    int num;
    int elementCount;
    GraphElement* elts;
    QTemporaryFile file;
};


#endif // GRAPHKRUSKAL_H
