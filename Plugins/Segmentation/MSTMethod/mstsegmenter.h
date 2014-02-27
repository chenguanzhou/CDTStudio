#ifndef MSTSEGMENTER_H
#define MSTSEGMENTER_H

#include "../../../Interfaces/cdtsegmentationInterface.h"
#include <QtPlugin>
#include <gdal_priv.h>

class MSTSegmenter : public CDTSegmentationInterface
{
    Q_OBJECT
    Q_INTERFACES(CDTSegmentationInterface)
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTSegmentationInterface" FILE "MSTMethod.json")
#endif // QT_VERSION >= 0x050000

public:
    MSTSegmenter(QObject *parent = 0);
    ~MSTSegmenter();

    QString     segmentationMethod()const ;
    QWidget*    paramsInterface  (QWidget* parent)const ;

    void run();

private:
    void clear();
    bool initialize();



    GDALDataset *poSrcDS;
    GDALDataset *poDstDS;
    std::vector<double> layerWeights;
};

#endif // MSTSEGMENTER_H
