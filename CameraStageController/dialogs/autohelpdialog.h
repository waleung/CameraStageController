#ifndef AUTOHELPDIALOG_H
#define AUTOHELPDIALOG_H

#include <QDialog>

namespace Ui {
class AutoHelpDialog;
}

class AutoHelpDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AutoHelpDialog(QWidget *parent = nullptr);
    ~AutoHelpDialog();

private:
    Ui::AutoHelpDialog *ui;
};

#endif // AUTOHELPDIALOG_H
