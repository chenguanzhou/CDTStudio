#ifndef CDTSEGMENTATIONINTERFACE_H
#define CDTSEGMENTATIONINTERFACE_H

#include <QtCore>
#include <QtPlugin>
#include <QProgressBar>
#include <QLabel>

class CDTSegmentationInterface:public QObject
{
    Q_OBJECT
public:
    explicit CDTSegmentationInterface(QObject* parent = 0):QObject(parent){}

    virtual QString segmentationMethod()const =0;
    virtual void startSegmentation(QProgressBar *progressBar,QLabel *label) =0;//TODO:Hide QProgressBar

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

    void setInputImagePath(const QString &path){inputImagePath=path;}
    void setMarkfilePath  (const QString &path){markfilePath=path;}
    void setShapefilePath (const QString &path){shapefilePath=path;}



//    QString inputImagePath()const{return _inputImagePath;}
//    QString markfilePath()  const{return _markfilePath;}
//    QString shapefilePath() const{return _shapefilePath;}
signals:
    void finished();

protected:
    QString inputImagePath;
    QString markfilePath;
    QString shapefilePath;
};

Q_DECLARE_INTERFACE(CDTSegmentationInterface,"cn.edu.WHU.CDTStudio.CDTSegmentationInterface/1.0")

#endif // CDTSEGMENTATIONINTERFACE_H
