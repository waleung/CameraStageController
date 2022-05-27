#include "stitcherhelpdialog.h"
#include "ui_stitcherhelpdialog.h"

StitcherHelpDialog::StitcherHelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StitcherHelpDialog)
{
    ui->setupUi(this);
}

StitcherHelpDialog::~StitcherHelpDialog()
{
    delete ui;
}
