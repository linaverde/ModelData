#ifndef GENERATEMODELPARAMSDIALOG_H
#define GENERATEMODELPARAMSDIALOG_H

#include <QDialog>

namespace Ui {
class GenerateModelParamsDialog;
}

class GenerateModelParamsDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateModelParamsDialog(QWidget *parent = nullptr);
    ~GenerateModelParamsDialog();

private:
    Ui::GenerateModelParamsDialog *ui;

private Q_SLOTS:
    void generateModel();
    void onMinFeatureValuesCountChanged();
    void onMinFeatureNormalValuesCountChanged();
    void onNimPeriodsCountChanged();
    void onMinPerionDurationChanged();
    void onMinValuesPerPeriodsChanged();
};

#endif // GENERATEMODELPARAMSDIALOG_H
