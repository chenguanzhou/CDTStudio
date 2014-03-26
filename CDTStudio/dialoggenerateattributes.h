#ifndef DIALOGGENERATEATTRIBUTES_H
#define DIALOGGENERATEATTRIBUTES_H

#include <QDialog>
#include <QMap>
#include "cdtsegmentationlayer.h"

namespace Ui {
class DialogGenerateAttributes;
}

class QModelIndex;
class QTreeWidgetItem;
class QListWidget;

class DialogGenerateAttributes : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int bandCount READ bandCount)

public:
    explicit DialogGenerateAttributes(CDTSegmentationLayer* segLayer,int nBandCount,QWidget *parent = 0);
    ~DialogGenerateAttributes();

    int bandCount()const;

private slots:
    void on_pushButtonRemoveAll_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButtonAddAll_clicked();
    void on_pushButtonGenerate_clicked();
    void on_treeWidgetAll_itemSelectionChanged();
    void onToolBoxSelectionChanged();
    void onShowWarningMessage(QString msg);
    void onFinished();

private:
    Ui::DialogGenerateAttributes *ui;
    CDTSegmentationLayer* segmentationLayer;
    int _bandCount;
    QMap<QString,QListWidget*> toolBoxWidgets;

    void loadPlugin();
    static int itemLevel(QTreeWidgetItem *item);
};

#endif // DIALOGGENERATEATTRIBUTES_H
