#ifndef DIALOGCATEGORYINFORMATION_H
#define DIALOGCATEGORYINFORMATION_H

#include <QDialog>

namespace Ui {
class DialogCategoryInformation;
}

class CategoryInformation
{
public:
    CategoryInformation(QString name=QString(),QColor clr=QColor()):categoryName(name),color(clr){}
    QString categoryName;
    QColor color;
};

QDataStream &operator <<(QDataStream &out,const CategoryInformation &categoryInformation);
QDataStream &operator >>(QDataStream &in, CategoryInformation &categoryInformation);

typedef QList<CategoryInformation> CDTCategoryInformationList;

class DialogCategoryInformation : public QDialog
{
    Q_OBJECT

public:
    explicit DialogCategoryInformation(CDTCategoryInformationList categoryInfo,
                                       bool isReadOnly=false,
                                       QWidget *parent = 0);
    friend class CDTImageLayer;
    ~DialogCategoryInformation();

private slots:
    void on_pushButton_clicked();
    void on_actionActionInsert_triggered();
    void on_actionActionRemove_triggered();
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void onCategoryNameChanged(const QString& name);
    void onAccepted();

private:
    Ui::DialogCategoryInformation *ui;
    CDTCategoryInformationList categoryInformationList;
    void initInfo();
    void setColor(const QColor& color);
};

#endif // DIALOGCATEGORYINFORMATION_H
