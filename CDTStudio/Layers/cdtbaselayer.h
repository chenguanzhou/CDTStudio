#ifndef CDTBASELAYER_H
#define CDTBASELAYER_H

#include <QObject>
#include <QUuid>
#include <log4qt/logger.h>

class QAction;
class QLabel;
class QStandardItem;
class QgsMapLayer;
class QgsMapCanvas;
class CDTProjectTreeItem;
class CDTFileSystem;
class CDTProjectLayer;

class CDTBaseLayer : public QObject
{
    Q_OBJECT
    LOG4QT_DECLARE_QCLASS_LOGGER
    Q_CLASSINFO("CDTBaseLayer","Basic information")
    Q_PROPERTY(QString Name READ name WRITE setName DESIGNABLE true USER true)
public:
    explicit CDTBaseLayer(QUuid uuid,QObject *parent = 0);
    virtual ~CDTBaseLayer();

    friend class CDTProjectWidget;
    friend class MainWindow;

    CDTProjectLayer     *rootProject()const;
    CDTProjectTreeItem  *keyItem()const;
    QStandardItem       *standardKeyItem()const;
    QgsMapLayer         *canvasLayer()const;
    QgsMapCanvas        *canvas()const;
    CDTFileSystem       *fileSystem()const;
    QUuid   id()const;
    QString name()const;
    QString tableName() const;

    ///Find and return the ancestor object of the this class's instace, whose name is className.
    QObject *getAncestor(const char* className);

protected:
    QList<QList<QAction *> > allActions()const;
    virtual void        onContextMenuRequest(QWidget *parent);

public slots:
    void setName(const QString &name);
    void rename();

protected slots:
    void setID(QUuid id);
    void setKeyItem(CDTProjectTreeItem *item);
    void setActions(QList<QList<QAction *> > actions);
    void setWidgetActions(QList<QPair<QLabel*,QWidget *> > actions);
    void setCanvasLayer(QgsMapLayer* layer);
    void setMapCanvas(QgsMapCanvas* canvas);

signals:
    void appendLayers(QList<QgsMapLayer*> layer);
    void removeLayer(QList<QgsMapLayer*> layer);
    void layerChanged();
    void nameChanged(QString);

private slots:
    void onMenuAboutToHide();


private:
    QUuid               uuid;
    CDTProjectTreeItem  *treeKeyItem;
    QgsMapLayer         *mapCanvasLayer;
    QgsMapCanvas        *mapCanvas;

    QList<QList<QAction *> > actions;
    QList<QPair<QLabel*,QWidget *> > widgetActions;
};

#endif // CDTBASELAYER_H
