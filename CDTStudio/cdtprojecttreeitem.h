#ifndef CDTPROJECTTREEITEM_H
#define CDTPROJECTTREEITEM_H

#include <QStandardItem>
#include "cdtbaseobject.h"

class CDTProjectTreeItem : public QStandardItem
{
public:
    enum Type{
        PROJECT_ROOT,
        IMAGE_ROOT,
        SEGMENTION_ROOT,
        SEGMENTION,
        CLASSIFICATION_ROOT,
        CLASSIFICATION,
        METHOD_PARAMS,
        PARAM,
        VALUE
    };

    CDTProjectTreeItem(Type tp, const QString &text,CDTBaseObject *crspdObject );
    Type getType()const {return type;}
    CDTBaseObject* getCorrespondingObject()const {return correspondingObject;}

private:
    void initAlignment();
    void initFont();
    void initColor();
    Type type;
    CDTBaseObject* correspondingObject;
};

#endif // CDTPROJECTTREEITEM_H
