#ifndef GENERATEDATADIALOG_H
#define GENERATEDATADIALOG_H

#include <QDialog>
#include <QFile>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>

namespace Ui {
class GenerateDataDialog;
}

class GenerateDataDialog : public QDialog
{
    Q_OBJECT

public:
    explicit GenerateDataDialog(QWidget *parent = nullptr);
    ~GenerateDataDialog();

private:
    Ui::GenerateDataDialog *ui;
    QJsonDocument doc;
    QJsonArray generatedData = QJsonArray();

    void jsonSchemaError();
    void generateClasses(int min, int max);
    void showGeneratedData();
    void showModelData();

private Q_SLOTS:
    void generateDatabase();
    void openFile();
    void save();
};

#endif // GENERATEDATADIALOG_H
