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
        CLASSIFICATION_ROOT,
        METHOD_PARAMS,
        PARAM,
        VALUE
    };

    CDTProjectTreeItem(Type type, const QString &text );


private:
    void initFont();
    void initColor();
};

#endif // CDTPROJECTTREEITEM_H
