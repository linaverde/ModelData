#include "restoremodeldialog.h"
#include "ui_restoremodeldialog.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QSet>

RestoreModelDialog::RestoreModelDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::RestoreModelDialog)
{
    ui->setupUi(this);
    connect(ui->btnOpenFile, SIGNAL(clicked()), this, SLOT(openFile()));
}

RestoreModelDialog::~RestoreModelDialog()
{
    delete ui;
}


void RestoreModelDialog::openFile(){
    QString filename = QFileDialog::getOpenFileName(this,
             tr("Open model file"), QDir::currentPath(), tr("GDB Files (*.gdb)"));
        ui->labelOpenedFile->setText(filename);

        QFile model(ui->labelOpenedFile->text());

        model.open(QIODevice::ReadOnly|QIODevice::Text);
        QByteArray data = model.readAll();
        model.close();

        QJsonParseError errorPtr;
        QJsonDocument doc;
        doc = QJsonDocument::fromJson(data, &errorPtr);
        if (doc.isNull()) {
            QMessageBox messageBox;
            messageBox.critical(0,"Ошибка","Невозможно прочитать файл - содержимое было повреждено или не соотвествует json-представлению данных");
            messageBox.setFixedSize(500,200);
            messageBox.show();
            return;
         }

        generateAlternatives(doc);

}

void RestoreModelDialog::generateAlternatives(QJsonDocument &doc){

   QJsonArray entries = doc.array();
   QJsonArray alternatives = QJsonArray();

   QJsonValue entry;
   foreach(entry, entries){
       QJsonObject curr = entry.toObject();
       QJsonObject alt = QJsonObject();
       alt.insert("class", curr.value("name").toString());
       QJsonArray features = curr.value("symptoms").toArray();
       QJsonValue symptom;
       QJsonArray featuresAlt = QJsonArray();
       foreach(symptom, features){
           QJsonObject symptomAlt = QJsonObject();
           symptomAlt.insert("feature", symptom.toObject().value("feature").toString());
           QJsonArray observations = symptom.toObject().value("observations").toArray();
           symptomAlt.insert("splits", generateObservationsAlternatives(observations));
           featuresAlt.append(symptomAlt);
       }
       alt.insert("features",featuresAlt);
       alternatives.append(alt);
   }

   if(alternatives.size() >0){
       // С помощью диалогового окна получаем имя файла с абсолютным путём
       QString saveFileName = QFileDialog::getSaveFileName(this,
                                                       tr("Save Splits (temporary file)"),
                                                       QString(),
                                                       tr("JSON (*.splits)"));
       QFileInfo fileInfo(saveFileName);   // С помощью QFileInfo
       QDir::setCurrent(fileInfo.path());  // установим текущую рабочую директорию, где будет файл, иначе может не заработать
       // Создаём объект файла и открываем его на запись
       QFile jsonFile(saveFileName);
       if (!jsonFile.open(QIODevice::WriteOnly))
       {
           return;
       }

       // Записываем текущий объект Json в файл
       jsonFile.write(QJsonDocument(alternatives).toJson(QJsonDocument::Indented));
       jsonFile.close();   // Закрываем файл
   }


}

bool isValid(QJsonArray split){
    if (split.size() == 0) return false;
    if (split.size() == 1) return true;

    for (int i = 0; i < split.size()-1; i++){
        QJsonArray arrCur = split.at(i).toArray();
        QSet<int> valuesCurrent = QSet<int>();
        for(int j =0; j < arrCur.size(); j++){
            valuesCurrent.insert(arrCur.at(j).toObject().value("value").toInt());
        }
        QJsonArray arrNext = split.at(i+1).toArray();
        QSet<int> valuesNext = QSet<int>();
        for(int j =0; j < arrNext.size(); j++){
            valuesNext.insert(arrNext.at(j).toObject().value("value").toInt());
        }
        if((valuesCurrent & valuesNext).size() != 0) return false;
    }

    return true;

}


QJsonArray subarray(QJsonArray observations, int start, int end){
    QJsonArray sub = QJsonArray();
    if (observations.size()>0 && start < end) {
        for(int i = start; i < end; i++){
            sub.append(i
                        //observations.at(i).toObject()
                        );
        }
    }
    return sub;
}


QJsonArray RestoreModelDialog::generateObservationsAlternatives(QJsonArray observations){
    QJsonArray splits = QJsonArray();
    if (observations.size() == 0) return QJsonArray();
    if (observations.size() == 1) {
        splits.append(observations);
        return splits;
    }

    for(int i = 0; i < observations.size(); i++){
        QJsonArray tail = QJsonArray();
        QJsonArray subsplits = QJsonArray();
        subsplits.append(subarray(observations, 0, observations.size()-i));
        subsplits.append(generateObservationsAlternatives(subarray(observations, i, observations.size())));
        splits.append(subsplits);
    }

    QJsonArray valid = QJsonArray();
    for(int i = 0; i < splits.size(); i++){
        if(isValid(splits.at(i).toArray())){
            valid.append(splits.at(i));
        }
    }


    return valid;
}


void RestoreModelDialog::mergeAlternatives(){

}
