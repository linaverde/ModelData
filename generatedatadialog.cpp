#include "generatedatadialog.h"
#include "ui_generatedatadialog.h"
#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonArray>
#include <QVector>
#include <string>
#include "qjsontablemodel.h"

GenerateDataDialog::GenerateDataDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GenerateDataDialog)
{
    ui->setupUi(this);
    connect(ui->btnGen, SIGNAL(clicked()), this, SLOT(generateDatabase()));
    connect(ui->btnOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
    connect(ui->btnSave, SIGNAL(clicked()), this, SLOT(save()));
}

void GenerateDataDialog::openFile(){
    QString filename = QFileDialog::getOpenFileName(this,
             tr("Open model file"), QDir::currentPath(), tr("GMS Files (*.gmc)"));
        ui->labelOpenedFile->setText(filename);

        QFile model(ui->labelOpenedFile->text());

        model.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray data = model.readAll();
        model.close();

        QJsonParseError errorPtr;
        doc = QJsonDocument::fromJson(data, &errorPtr);
        if (doc.isNull()) {
            jsonSchemaError();
            return;
         }

        showModelData();
}

void GenerateDataDialog::generateDatabase(){

    int minValues = ui->minValuesPerClassCountSpinBox->value();
    int maxValues = ui->maxValuesPerClassCountSpinBox->value();

    if(minValues > maxValues){
        QMessageBox messageBox;
        messageBox.critical(0,"Ошибка","Максимальное количество классов должно быть больше или равно минимальному");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }
    generateClasses(minValues, maxValues);
}

void GenerateDataDialog::generateClasses(int min, int max){

    if (doc.isNull()){
        QMessageBox messageBox;
        messageBox.critical(0,"Ошибка","Файл модели данных не выбран или был удален");
        messageBox.setFixedSize(500,200);
        messageBox.show();
        return;
    }


    while(generatedData.count()) {
        generatedData.pop_back();
    }

    QJsonArray classes = doc.object().value("classes").toArray();

    int minObservationMomentsPerPeriod = 1;
    int maxExtraObservationMoments = 2;

    QJsonArray data = QJsonArray();

    for(int classIterator = 0; classIterator < classes.size(); classIterator++){
        int actualValue = min + rand() % max;
        QJsonObject currentClass = classes.at(classIterator).toObject();
        for (int itemIterator = 0; itemIterator < actualValue; itemIterator++) {
            QJsonObject item = QJsonObject();
            item.insert("name", currentClass.value("name").toString());
            QJsonArray symptoms = QJsonArray();
            for (int featureIterator = 0; featureIterator < currentClass.value("symptoms").toArray().size(); featureIterator ++) {
                QJsonArray observations = QJsonArray();
                QJsonObject currentSymptom = currentClass.value("symptoms").toArray().at(featureIterator).toObject();
                QJsonObject symptomData = QJsonObject();
                symptomData.insert("feature", currentSymptom.value("feature"));
                QVector<int> durations;
                for(int period = 0; period <currentSymptom.value("periods").toArray().size(); period++){
                    QVector<int> observationsMoments;
                    int minDuration = currentSymptom.value("periods").toArray().at(period).toObject().value("durationMin").toInt();
                    int maxDuration = currentSymptom.value("periods").toArray().at(period).toObject().value("durationMax").toInt();
                    durations.append(minDuration + rand() % maxDuration);
                    int offset = 0;
                    if (period != 0){
                        int durationcurr;
                        foreach(durationcurr, durations){
                            offset += durationcurr;
                        }
                    }
                    observationsMoments.append(offset + rand() % (durations[period] + offset));
                    for (int i =0; i < maxExtraObservationMoments; i++){
                        if (rand()%2 == 0 && durations[period]-minObservationMomentsPerPeriod > i + minObservationMomentsPerPeriod){
                            int newMonment = offset + rand() % (durations[period] + offset);
                            if (!observationsMoments.contains(newMonment)) {
                                observationsMoments.append(newMonment);
                            }
                        }
                    }
                    qSort(observationsMoments);
                    QJsonArray possibleValues = currentSymptom.value("periods").toArray().at(period).toObject().value("values").toArray();
                    for(int moment = 0; moment < observationsMoments.size(); moment++){
                        QJsonObject current = QJsonObject();
                        current.insert("time", observationsMoments.at(moment));
                        int value = possibleValues.at(rand() % possibleValues.size()).toInt();
                        current.insert("value", value);
                        observations.append(current);
                    }
                }
                symptomData.insert("observations", observations);
                symptoms.append(symptomData);
            }
            item.insert("symptoms", symptoms);

            data.append(item);
        }
    }

    this->generatedData = data;
    showGeneratedData();
}

void GenerateDataDialog::save() {
    if(generatedData.size() >0){
        // С помощью диалогового окна получаем имя файла с абсолютным путём
        QString saveFileName = QFileDialog::getSaveFileName(this,
                                                        tr("Save GeneratedDatabase"),
                                                        QString(),
                                                        tr("JSON (*.gdb)"));
        QFileInfo fileInfo(saveFileName);   // С помощью QFileInfo
        QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл, иначе может не заработать
        // Создаём объект файла и открываем его на запись
        QFile jsonFile(saveFileName);
        if (!jsonFile.open(QIODevice::WriteOnly))
        {
            return;
        }

        // Записываем текущий объект Json в файл
        jsonFile.write(QJsonDocument(generatedData).toJson(QJsonDocument::Indented));
        jsonFile.close();   // Закрываем файл
    }
}

void GenerateDataDialog::showGeneratedData(){
    QJsonArray tableData = QJsonArray();
    QJsonValue entry;
    foreach(entry, generatedData){
        QJsonObject entryObj = entry.toObject();
        QJsonArray symptoms = entryObj.value("symptoms").toArray();
        QJsonValue symptom;
        foreach(symptom, symptoms){
            QJsonObject symptomObj = symptom.toObject();
            QJsonArray observations = symptomObj.value("observations").toArray();
            QJsonValue observation;
            foreach(observation, observations){
                QJsonObject moment = observation.toObject();
                QJsonObject tableEntry = QJsonObject();
                tableEntry.insert("class", entryObj.value("name").toString());
                tableEntry.insert("symptom", symptomObj.value("feature").toString());
                tableEntry.insert("time", moment.value("time").toInt());
                tableEntry.insert("value", moment.value("value").toInt());
                tableData.append(tableEntry);
            }
        }

    }

    QJsonTableModel::Header header;
    header.push_back( QJsonTableModel::Heading( { {"title","Класс"},    {"index","class"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","Признак"},   {"index","symptom"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","Время замера"},  {"index","time"} }) );
    header.push_back( QJsonTableModel::Heading( { {"title","Значение"}, {"index","value"} }) );

    QJsonTableModel *table = new QJsonTableModel( header, this );
    ui->tableGeneratedDatabase->setModel( table );

    QJsonDocument doc;
    doc.setArray(tableData);

    QJsonDocument jsonDocument = QJsonDocument::fromJson( doc.toJson() );
    table->setJson( jsonDocument );

}

void GenerateDataDialog::showModelData(){
    QJsonArray tableClassesData = QJsonArray();
    QJsonObject model = doc.object();
    QJsonArray classes = model.value("classes").toArray();
    QJsonValue classEntry;
    foreach(classEntry, classes){
        QJsonObject classObj = classEntry.toObject();
        QJsonArray features = classObj.value("symptoms").toArray();
        QJsonValue feature;
        foreach(feature, features){
            QJsonObject featureObj = feature.toObject();
            QJsonArray periods = featureObj.value("periods").toArray();
            for(int i = 0; i < periods.size(); i++){
                QJsonObject periodObj = periods.at(i).toObject();
                QJsonArray values = periodObj.value("values").toArray();
                QJsonValue value;
                std::string valuesStr = "";
                foreach(value, values){
                    valuesStr += std::to_string(value.toInt())+ " ";
                }
                QJsonObject tableEntry = QJsonObject();
                tableEntry.insert("class", classObj.value("name").toString());
                tableEntry.insert("feature", featureObj.value("feature").toString());
                tableEntry.insert("period_index", i);
                std::string duration = "";
                duration = duration + std::to_string(periodObj.value("durationMin").toInt());
                duration = duration + "-";
                duration = duration + std::to_string(periodObj.value("durationMax").toInt());
                tableEntry.insert("duration", QString::fromStdString(duration));
                tableEntry.insert("values", QString::fromStdString(valuesStr));
                tableClassesData.append(tableEntry);
            }
        }
    }

    QJsonTableModel::Header headerClasses;
    headerClasses.push_back( QJsonTableModel::Heading( { {"title","Класс"},    {"index","class"} }) );
    headerClasses.push_back( QJsonTableModel::Heading( { {"title","Признак"},   {"index","feature"} }) );
    headerClasses.push_back( QJsonTableModel::Heading( { {"title","Номер периода"},  {"index","period_index"} }) );
    headerClasses.push_back( QJsonTableModel::Heading( { {"title","Длина периода"}, {"index","duration"} }) );
    headerClasses.push_back( QJsonTableModel::Heading( { {"title","Значения"}, {"index","values"} }) );

    QJsonTableModel *tableClasses = new QJsonTableModel( headerClasses, this );
    ui->tableModelClasses->setModel( tableClasses );

    QJsonDocument docClasses;
    docClasses.setArray(tableClassesData);

    QJsonDocument jsonDocumentClasses = QJsonDocument::fromJson( docClasses.toJson() );
    tableClasses->setJson( jsonDocumentClasses );


    QJsonArray tableFeaturesData = QJsonArray();
    QJsonArray features = model.value("features").toArray();
    QJsonValue feature;
    foreach(feature, features){
        QJsonObject featureObj = feature.toObject();
        QJsonArray values = featureObj.value("values").toArray();
        QJsonValue value;
        std::string valuesStr = "";
        foreach(value, values){
            valuesStr = valuesStr + std::to_string(value.toInt()) + " ";
        }
        QJsonArray normal = featureObj.value("normal_values").toArray();
        QJsonValue normValue;
        std::string normValuesStr = "";
        foreach(normValue, normal){
            normValuesStr = normValuesStr + std::to_string(normValue.toInt())+ " ";
        }
        QJsonObject tableEntry = QJsonObject();
        tableEntry.insert("feature", featureObj.value("name").toString());
        tableEntry.insert("values", QString::fromStdString(valuesStr));
        tableEntry.insert("normal", QString::fromStdString(normValuesStr));
        tableFeaturesData.append(tableEntry);
    }

    QJsonTableModel::Header headerFeatures;
    headerFeatures.push_back( QJsonTableModel::Heading( { {"title","Признак"},    {"index","feature"} }) );
    headerFeatures.push_back( QJsonTableModel::Heading( { {"title","Значения"},   {"index","values"} }) );
    headerFeatures.push_back( QJsonTableModel::Heading( { {"title","Нормальные значения"},  {"index","normal"} }) );

    QJsonTableModel *tableFeatures = new QJsonTableModel( headerFeatures, this );
    ui->tableModelFeatures->setModel( tableFeatures );

    QJsonDocument docFeatures;
    docFeatures.setArray(tableFeaturesData);

    QJsonDocument jsonDocumentFeatures = QJsonDocument::fromJson( docFeatures.toJson() );
    tableFeatures->setJson( jsonDocumentFeatures );

}

void GenerateDataDialog::jsonSchemaError(){
    QMessageBox messageBox;
    messageBox.critical(0,"Ошибка","Невозможно прочитать файл - содержимое было повреждено или не соотвествует json-представлению модели");
    messageBox.setFixedSize(500,200);
    messageBox.show();
    return;
}

GenerateDataDialog::~GenerateDataDialog()
{
    delete ui;
}
