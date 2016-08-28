#ifndef DIALOGABOUT_H
#define DIALOGABOUT_H

#include <QDialog>

namespace Ui {
class DialogAbout;
}

class DialogAbout : public QDialog
{
    Q_OBJECT

public:
    explicit DialogAbout(QWidget *parent = 0);
    ~DialogAbout();

public slots:
    void onOfficialPageLinkClicked();
    void onDocumentationLinkClicked();
    void onTable3rdPartyItemClicked(QModelIndex index);

private:
    Ui::DialogAbout *ui;
};

#endif // DIALOGABOUT_H
