#include "restoremodeldialog.h"
#include "ui_restoremodeldialog.h"

RestoreModelDialog::RestoreModelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestoreModelDialog)
{
    ui->setupUi(this);
}

RestoreModelDialog::~RestoreModelDialog()
{
    delete ui;
}
