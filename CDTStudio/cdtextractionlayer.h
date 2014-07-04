#ifndef CDTEXTRACTIONLAYER_H
#define CDTEXTRACTIONLAYER_H

#include "cdtbaseobject.h"

class QColor;
class QAction;
class QWidgetAction;

class CDTExtractionLayer : public CDTBaseObject
{
    Q_OBJECT

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
    void exportShapefile();

private slots:
    void setEditState(EDITSTATE state);
    void setGeometryModified(bool modified);

    void onActionStartEdit();
    void onActionRollBack();
    void onActionSave();
    void onActionStop();

private:
    void start();
    void rollback();
    void save();
    void stop();

signals:
    void nameChanged();
    void extractionChanged();
    void removeExtraction(CDTExtractionLayer*);

private:
    bool isGeometryModified;
    EDITSTATE currentEditState;

    QWidgetAction *actionChangeColor;
    QAction *actionRemoveExtraction;
    QAction *actionRename;
    QAction *actionExportShapefile;

    QAction *actionStartEdit;
    QAction *actionRollBack;
    QAction *actionSave;
    QAction *actionStop;

    static QList<CDTExtractionLayer *> layers;
};

QDataStream &operator<<(QDataStream &out,const CDTExtractionLayer &extraction);
QDataStream &operator>>(QDataStream &in, CDTExtractionLayer &extraction);

#endif // CDTEXTRACTIONLAYER_H
