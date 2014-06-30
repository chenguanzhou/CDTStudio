#ifndef CDTEXTRACTIONLAYER_H
#define CDTEXTRACTIONLAYER_H

#include "cdtbaseobject.h"

class QColor;
class QAction;
class QWidgetAction;

class CDTExtractionLayer : public CDTBaseObject
{
    Q_OBJECT
    Q_PROPERTY(EDITSTATE editState READ editState WRITE setEditState NOTIFY editStateChanged)

public:
    enum EDITSTATE{
        LOCKED,EDITING
    };
    explicit CDTExtractionLayer(QUuid uuid,QObject *parent);
    ~CDTExtractionLayer();

    friend QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
    friend QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

    QString name()const;
    QString shapefilePath() const;
    QColor  color()const;
    QString imagePath()const;

    EDITSTATE editState()const;

    static QList<CDTExtractionLayer *> getLayers();
    static CDTExtractionLayer * getLayer(QUuid id);

public slots:
    void onContextMenuRequest(QWidget *parent);
    void setName(const QString& name);
    void setBorderColor(const QColor &clr);
    void initLayer(const QString& name,
            const QString &shpPath,
            const QColor &color);
    void rename();
    void remove();
    void setEditState(EDITSTATE state);

private slots:
    void onEditStateChanged();

signals:
    void nameChanged();
    void extractionChanged();
    void editStateChanged();
    void removeExtraction(CDTExtractionLayer*);

private:
    bool isGeometryChanged;
    EDITSTATE currentEditState;

    QWidgetAction *actionChangeColor;
    QAction *actionRemoveExtraction;
    QAction *actionRename;

    QAction *actionStartEdit;
    QAction *actionRollBack;
    QAction *actionSave;
    QAction *actionStop;

    static QList<CDTExtractionLayer *> layers;
};

QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

#endif // CDTEXTRACTIONLAYER_H
