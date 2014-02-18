#include "cdtprojecttreeitem.h"

CDTProjectTreeItem::CDTProjectTreeItem(Type tp, const QString &text)
    :type(tp)
{
    setText(text);
    initAlignment();
    initFont();
    initColor();
}

void CDTProjectTreeItem::initAlignment()
{
    switch (type) {
    case PARAM:
        this->setTextAlignment(Qt::AlignRight);
        break;
    }
}

void CDTProjectTreeItem::initFont()
{
    QFont font= this->font();
    switch (type) {
    case PROJECT_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize()+3);
        break;
    case IMAGE_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize()+2);
        break;
    case SEGMENTION_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize()+1);
        break;
    case CLASSIFICATION_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize());
        break;
    case METHOD_PARAMS:
        font.setBold(true);
        break;
    case PARAM:
        font.setUnderline(true);
        break;
    default:
        break;
    }
    this->setFont(font);
}

void CDTProjectTreeItem::initColor()
{
    QBrush brush = this->foreground();
    switch (type) {
    case PROJECT_ROOT:
        break;
    case IMAGE_ROOT:
        brush.setColor(QColor(Qt::red));
        break;
    case METHOD_PARAMS:
        break;
    case PARAM:
        break;
    default:
        break;
    }
    this->setForeground(brush);
}
