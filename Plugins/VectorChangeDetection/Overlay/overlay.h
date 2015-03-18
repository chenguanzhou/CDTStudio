#ifndef OVERLAY_H
#define OVERLAY_H

#include "cdtvectorchangedetectioninterface.h"

class Overlay:public CDTVectorChangeDetectionInterface
{
    Q_OBJECT
#if QT_VERSION >= 0x050000
    Q_PLUGIN_METADATA(IID "cn.edu.WHU.CDTStudio.CDTVectorChangeDetectionInterface" FILE "Overlay.json")
#else
    Q_INTERFACES(CDTVectorChangeDetectionInterface)
#endif // QT_VERSION >= 0x050000

public:
    explicit Overlay(QObject* parent = 0);
    ~Overlay();

    QString methodName()const ;
    void detect(QgsFeatureIterator iteratorT1,QgsFeatureIterator iteratorT2,QString fieldNameT1,QString fieldNameT2,QProgressBar *progressBar,QLabel *label);
};

#endif // OVERLAY_H
