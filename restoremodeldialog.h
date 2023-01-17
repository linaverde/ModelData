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
    void generateAlternatives(QJsonDocument &doc);
    void mergeAlternatives();
    QJsonArray generateObservationsAlternatives(QJsonArray observations);

private Q_SLOTS:
    void openFile();
};

#endif // RESTOREMODELDIALOG_H
