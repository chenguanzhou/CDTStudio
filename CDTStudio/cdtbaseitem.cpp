#include "cdtbaseitem.h"

CDTBaseItem::CDTBaseItem(QString name, QString path, CDTBaseItem *parent)
    :QObject(parent),rootItem(new QStandardItem(name)),valueItem(new QStandardItem(path))
{
    if (parent)
        connect(this,SIGNAL(itemChanged()),parent,SIGNAL(itemChanged()));
}

QDataStream &operator>>(QDataStream &in, CDTBaseItem &item)
{
    QString className;
    in>>className;
    item = QObject::staticMetaObject.newInstance() ;

    QMap<QString,QVariant> properties;
    in>>properties;
    QStringList names = properties.keys();
    foreach (QString name, names) {
        QVariant value = properties.value(name);
        item.setProperty(name.toAscii(),value);        
    }

    int childItemCount=0;
    in>>childItemCount;
    for (int i=0;i<childItemCount;++i)
    {
        CDTBaseItem *childItem;
        in>>(*childItem);
        childItem->setParent(&item);
    }
    return in;
}

QDataStream &operator<<(QDataStream &out, const CDTBaseItem &item)
{
    out<<QString(item.metaObject()->className());
    QMap<QString,QVariant> properties;
    for (int i=0;i<item.metaObject()->propertyCount();++i)
    {
        QMetaProperty property = item.metaObject()->property(i);
        QVariant value = item.property(property.name());
        properties.insert(property.name(),value);
    }
    out<<properties;    

    QList<CDTBaseItem *> childrenItems = item.findChildren<CDTBaseItem*>();
    out<<childrenItems.size();
    foreach (CDTBaseItem *childItem, childrenItems) {        
        out<<(*childItem);
    }
    return out;
}
