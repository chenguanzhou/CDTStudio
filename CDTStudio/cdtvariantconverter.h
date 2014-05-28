#ifndef QDATATOVARIANT_H
#define QDATATOVARIANT_H

#include <QVariant>

template <typename T>
QVariant dataToVariant(const T& data)
{
    QByteArray array;
    QDataStream stream(&array,QIODevice::WriteOnly);
    stream<<data;
    return QVariant(array);
}

template <typename T>
T variantToData(const QVariant& variant)
{
    T result;
    QByteArray array = variant.toByteArray();
    QDataStream stream(&array,QIODevice::ReadOnly);
    stream>>result;
    return result;
}


#endif // QDATATOVARIANT_H
