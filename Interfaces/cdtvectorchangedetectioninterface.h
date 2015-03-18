#ifndef CDTVECTORCHANGEDETECTIONINTERFACE_H
#define CDTVECTORCHANGEDETECTIONINTERFACE_H

#include <QtCore>
#include <QtPlugin>
#include <QProgressBar>
#include <QLabel>
#include <qgsfeatureiterator.h>

class CDTVectorChangeDetectionInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTVectorChangeDetectionInterface(QObject* parent = 0):QObject(parent){}

    virtual QString methodName()const =0;
    virtual void detect(QgsFeatureIterator iteratorT1,QgsFeatureIterator iteratorT2,QString fieldNameT1,QString fieldNameT2,QProgressBar *progressBar,QLabel *label) =0;

    QVariantMap params() const
    {
        QMap<QString,QVariant> params;
        const QMetaObject *metaObj = this->metaObject();
        for(int i=metaObj->propertyOffset();i<metaObj->propertyCount();++i)
        {
            QMetaProperty property = metaObj->property(i);
            if (property.isUser() && property.isDesignable())
            {
                params.insert(property.name(),this->property(property.name()));
            }
        }
        return params;
    }

signals:
    void finished();

};

Q_DECLARE_INTERFACE(CDTVectorChangeDetectionInterface,"cn.edu.WHU.CDTStudio.CDTVectorChangeDetectionInterface/1.0")

#endif // CDTVECTORCHANGEDETECTIONINTERFACE_H
