#ifndef DIALOGGENERATEATTRIBUTES_H
#define DIALOGGENERATEATTRIBUTES_H

#include <QDialog>

namespace Ui {
class DialogGenerateAttributes;
}

class QModelIndex;
class QTreeWidgetItem;

class DialogGenerateAttributes : public QDialog
{
    Q_OBJECT
    Q_PROPERTY(int bandCount READ bandCount)

public:
    explicit DialogGenerateAttributes(int nBandCount,QWidget *parent = 0);
    ~DialogGenerateAttributes();

    int bandCount()const;

private slots:
    void on_pushButtonRemoveAll_clicked();
    void on_pushButtonRemove_clicked();
    void on_pushButtonAdd_clicked();
    void on_pushButtonAddAll_clicked();
    void on_pushButtonGenerate_clicked();
    void on_treeWidgetAll_itemSelectionChanged();
    void on_treeWidgetSelect_itemSelectionChanged();

private:
    Ui::DialogGenerateAttributes *ui;
    int _bandCount;

    void loadPlugin();
    static int itemLevel(QTreeWidgetItem *item);
};

#endif // DIALOGGENERATEATTRIBUTES_H
