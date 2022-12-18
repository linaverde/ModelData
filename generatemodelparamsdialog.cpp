#include "generatemodelparamsdialog.h"
#include "ui_generatemodelparamsdialog.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonArray>
#include <QJsonObject>
#include <string>
#include <QFileDialog>
#include <QList>
#include <algorithm>
#include <QMessageBox>

GenerateModelParamsDialog::GenerateModelParamsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateModelParamsDialog)
{
    ui->setupUi(this);
    connect(ui->btnGen, SIGNAL(clicked()), this, SLOT(generateModel()));
    connect(ui->spinBoxFeatureValuesCountMin, SIGNAL(valueChanged()), this, SLOT(onMinFeatureValuesCountChanged()));
    connect(ui->spinBoxFeatureValuesCountMinNormal, SIGNAL(valueChanged()), this, SLOT(onMinFeatureNormalValuesCountChanged()));
    connect(ui->spinBoxPeriodsCountMin, SIGNAL(valueChanged()), this, SLOT(onNimPeriodsCountChanged()));
    connect(ui->spinBoxPeriodTimeMin, SIGNAL(valueChanged()), this, SLOT(onMinPerionDurationChanged()));
    connect(ui->spinBoxFeatureValuesCountForPeriodMin, SIGNAL(valueChanged()), this, SLOT(onMinValuesPerPeriodsChanged()));
}

GenerateModelParamsDialog::~GenerateModelParamsDialog()
{
    delete ui;
}

void GenerateModelParamsDialog::generateModel(){
    QJsonObject schema;
    QJsonArray features;
    for (int i = 0; i < ui->spinBoxFeatures->value(); ++i) {
        //add feature name indexed
        QJsonObject feature;
        QString name = "feature";
        name += QString::number(i);
        feature.insert("name", QJsonValue::fromVariant(name));
        //generate values
        QJsonArray values;
        int minValuesCount = ui->spinBoxFeatureValuesCountMin->value();
        int maxValuesCount = ui->spinBoxFeatureValuesCountMax->value();
        int valuesCount = minValuesCount + (rand() % static_cast<int>(maxValuesCount - minValuesCount + 1));
        for (int j = 0; j < valuesCount; ++j) {
            values.append(j);
        }
        feature.insert("values", values);
        //generate normal values
        QJsonArray normalValues;
        int minValuesCountN = ui->spinBoxFeatureValuesCountMinNormal->value();
        int maxValuesCountN = ui->spinBoxFeatureValuesCountMaxNormal->value();
        int valuesCountN = minValuesCountN + (rand() % static_cast<int>(maxValuesCountN - minValuesCountN + 1));
        QList<int> normalValuesIndexes;
        while(normalValuesIndexes.length() < valuesCountN){
            int newIndex = (rand() % static_cast<int>(valuesCount + 1));
            if(!normalValuesIndexes.contains(newIndex)){
                normalValuesIndexes.append(newIndex);
            }
        }
        for (int j = 0; j <normalValuesIndexes.length(); ++j){
            normalValues.append(values[j]);
        }
        feature.insert("normal_values", normalValues);
        features.append(feature);
    }

    schema.insert("features", features);
    QJsonArray classes;
    //generate classes
    for (int i = 0; i < ui->spinBoxClasses->value(); ++i){
        //add feature name indexed
        QJsonObject classItem;
        QString name = "class";
        name += QString::number(i);
        classItem.insert("name", QJsonValue::fromVariant(name));
        //generate features
        QJsonArray symptomps;
        for (int j = 0; j < features.count(); j++){
            QJsonObject feature;
            feature.insert("feature", features.at(j).toObject().value("name").toString());

            //generate periods
            QJsonArray periods;
            int minPediodsCount = ui->spinBoxPeriodsCountMin->value();
            int maxPeriondsCout = ui->spinBoxPeriodsCountMax->value();
            int periodsCount = minPediodsCount + (rand() % static_cast<int>(maxPeriondsCout - minPediodsCount + 1));
            int minDuration = ui->spinBoxPeriodTimeMin->value();
            int maxDuration = ui->spinBoxPeriodTimeMax->value();
            int featureValuesCount = features.at(j).toObject().value("values").toArray().size();
            QJsonArray possibleValues = features.at(j).toObject().value("values").toArray();
            for (int p = 0; p< periodsCount; p++){
                QJsonObject periodItem;
                QPair<int, int> duration;
                duration.first = minDuration + (rand() % static_cast<int>(maxDuration - minDuration + 1));
                duration.second = minDuration + (rand() % static_cast<int>(maxDuration - minDuration + 1));
                periodItem.insert("durationMin", std::min(duration.first, duration.second));
                periodItem.insert("durationMax", std::max(duration.first, duration.second));
                //generate values
                QJsonArray values;
                int minValuesPerPeriodCount = ui->spinBoxFeatureValuesCountForPeriodMin->value();
                int maxValuesPerPeriodCount = ui->spinBoxFeatureValuesCountForPeriodMax->value();
                int valuesPerPeriodCount = minValuesPerPeriodCount + (rand() % static_cast<int>(std::min(maxValuesPerPeriodCount, featureValuesCount) - minValuesPerPeriodCount + 1));
                QJsonArray previousPeriodValues;
                if(p > 0){
                    previousPeriodValues = periods.at(p-1).toObject().value("values").toArray();
                    if (possibleValues.size()-previousPeriodValues.size() < minValuesPerPeriodCount){
                        QMessageBox messageBox;
                        messageBox.critical(0,"Ошибка","Невозможно сгенерировать модель");
                        messageBox.setFixedSize(500,200);
                        messageBox.show();
                        return;
                    } else {
                        valuesPerPeriodCount = std::min(valuesPerPeriodCount, possibleValues.size()-previousPeriodValues.size());
                    }
                }
                while(values.count() < valuesPerPeriodCount){
                    int newIndex = (rand() % static_cast<int>(featureValuesCount));
                    if(!previousPeriodValues.contains(possibleValues[newIndex]) && !values.contains(possibleValues[newIndex])){
                        values.append(possibleValues[newIndex]);
                    }
                }
                periodItem.insert("values", values);
                periods.push_back(periodItem);
            }
            feature.insert("periods", periods);
            symptomps.append(feature);

        }
        classItem.insert("symptoms", symptomps);
        classes.append(classItem);
    }
    schema.insert("classes", classes);

    //save config json

    // С помощью диалогового окна получаем имя файла с абсолютным путём
    QString saveFileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save GenerateModelConfig"),
                                                        QString(),
                                                        tr("JSON (*.gmc)"));
    QFileInfo fileInfo(saveFileName);   // С помощью QFileInfo
    QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл, иначе может не заработать
    // Создаём объект файла и открываем его на запись
    QFile jsonFile(saveFileName);
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        return;
    }

    // Записываем текущий объект Json в файл
    jsonFile.write(QJsonDocument(schema).toJson(QJsonDocument::Indented));
    jsonFile.close();   // Закрываем файл

}

void GenerateModelParamsDialog::onMinFeatureValuesCountChanged(){
    ui->spinBoxFeatureValuesCountMax->setMinimum(ui->spinBoxFeatureValuesCountMin->value());
}

void GenerateModelParamsDialog::onMinFeatureNormalValuesCountChanged(){
    ui->spinBoxFeatureValuesCountMaxNormal->setMinimum(ui->spinBoxFeatureValuesCountMinNormal->value());
}

void GenerateModelParamsDialog::onNimPeriodsCountChanged(){
    ui->spinBoxPeriodsCountMax->setMinimum(ui->spinBoxPeriodsCountMin->value());
}

void GenerateModelParamsDialog::onMinPerionDurationChanged(){
    ui->spinBoxPeriodTimeMax->setMinimum(ui->spinBoxPeriodTimeMin->value());
}

void GenerateModelParamsDialog::onMinValuesPerPeriodsChanged(){
    ui->spinBoxFeatureValuesCountForPeriodMax->setMinimum(ui->spinBoxFeatureValuesCountForPeriodMin->value());
}
