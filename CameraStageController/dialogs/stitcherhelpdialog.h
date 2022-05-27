#ifndef STITCHERHELPDIALOG_H
#define STITCHERHELPDIALOG_H

#include <QDialog>

namespace Ui {
class StitcherHelpDialog;
}

class StitcherHelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit StitcherHelpDialog(QWidget *parent = nullptr);
    ~StitcherHelpDialog();

private:
    Ui::StitcherHelpDialog *ui;
};

#endif // STITCHERHELPDIALOG_H
