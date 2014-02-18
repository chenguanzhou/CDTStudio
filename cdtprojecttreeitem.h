#ifndef CDTPROJECTTREEITEM_H
#define CDTPROJECTTREEITEM_H

#include <QStandardItem>

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

    CDTProjectTreeItem(Type tp, const QString &text );


private:
    void initAlignment();
    void initFont();
    void initColor();
    Type type;
};

#endif // CDTPROJECTTREEITEM_H
