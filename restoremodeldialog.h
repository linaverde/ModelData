#ifndef RESTOREMODELDIALOG_H
#define RESTOREMODELDIALOG_H

#include <QDialog>

namespace Ui {
class RestoreModelDialog;
}

class RestoreModelDialog : public QDialog
{
    Q_OBJECT

public:
    explicit RestoreModelDialog(QWidget *parent = nullptr);
    ~RestoreModelDialog();

private:
    Ui::RestoreModelDialog *ui;
};

#endif // RESTOREMODELDIALOG_H
