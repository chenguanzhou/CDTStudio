#ifndef CDTCLASSIFIERINTERFACE_H
#define CDTCLASSIFIERINTERFACE_H

#include <QtCore>
#include <QtPlugin>

namespace cv {
class Mat;
}

class CDTClassifierInterface:public QObject
{
    Q_OBJECT    
public:
    explicit CDTClassifierInterface(QObject* parent = 0):QObject(parent){}    

    virtual QString classifierName() const = 0;
    virtual cv::Mat startClassification(const cv::Mat &data,const cv::Mat &train_data,const cv::Mat &responses) = 0;
    QMap<QString,QVariant> params() const
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

private:    
};

Q_DECLARE_INTERFACE(CDTClassifierInterface,"cn.edu.WHU.CDTStudio.CDTClassifierInterface/1.0")

#endif // CDTCLASSIFIERINTERFACE_H
