#ifndef DIALOGGENERATEATTRIBUTES_H
#define DIALOGGENERATEATTRIBUTES_H

#include <QDialog>
#include <QMap>

namespace Ui {
class DialogGenerateAttributes;
}

class QModelIndex;
class QTreeWidgetItem;
class QListWidget;
class CDTAttributeGenerator;
struct QUuid;

class DialogGenerateAttributes : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int bandCount READ bandCount)

public:
    explicit DialogGenerateAttributes(QUuid segmentationID,int nBandCount,QWidget *parent = 0);
    ~DialogGenerateAttributes();

    int bandCount()const;

private slots:
    void on_pushButtonRemoveAll_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButtonAddAll_clicked();
    void on_pushButtonGenerate_clicked();
    void on_pushButtonCancel_clicked();
    void on_treeWidgetAll_itemSelectionChanged();
    void onToolBoxSelectionChanged();
    void onShowWarningMessage(QString msg);
    void onFinished();


private:
    Ui::DialogGenerateAttributes *ui;
    QUuid segID;
    int _bandCount;
    QMap<QString,QListWidget*> toolBoxWidgets;    
    CDTAttributeGenerator* attributeGenerator;

    void showPlugins();
    static int itemLevel(QTreeWidgetItem *item);
    void setWorkingUI();
};

#endif // DIALOGGENERATEATTRIBUTES_H
