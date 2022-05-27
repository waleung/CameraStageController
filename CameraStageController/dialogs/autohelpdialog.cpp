#include "autohelpdialog.h"
#include "ui_autohelpdialog.h"

AutoHelpDialog::AutoHelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::AutoHelpDialog)
{
    ui->setupUi(this);
}

AutoHelpDialog::~AutoHelpDialog()
{
    delete ui;
}
