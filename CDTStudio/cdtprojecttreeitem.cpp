#include "cdtprojecttreeitem.h"

CDTProjectTreeItem::CDTProjectTreeItem(CDTItemType tp, LayerType ly, const QString &text, CDTBaseObject *crspdObject)
    :_itemType(tp),layerType(ly),corrObject(crspdObject),qgsMapLayer(NULL)
{
    setText(text);
    initAlignment();
    initFont();
    initColor();
    initCheckState();
}

QgsMapLayer *CDTProjectTreeItem::mapLayer() const
{
    return qgsMapLayer;
}

void CDTProjectTreeItem::setMapLayer(QgsMapLayer *layer)
{
    qgsMapLayer = layer;
}

void CDTProjectTreeItem::initAlignment()
{
    switch (_itemType) {
    case PARAM:
    case METHOD_PARAMS:
        this->setTextAlignment(Qt::AlignRight);
        break;
    }
}

void CDTProjectTreeItem::initFont()
{
    QFont font= this->font();
    switch (_itemType) {
    case PROJECT_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize()+3);        
        break;
    case IMAGE_ROOT:
        font.setBold(true);
        font.setPointSize(font.pointSize()+2);
        break;
    case EXTRACTION_ROOT:
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
    switch (_itemType) {
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

void CDTProjectTreeItem::initCheckState()
{
    if (layerType != EMPTY)
    {
        this->setCheckable(true);
        this->setCheckState(Qt::Checked);
    }
    else
        this->setCheckable(false);
}
