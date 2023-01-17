#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "generatemodelparamsdialog.h"
#include "generatedatadialog.h"
#include "restoremodeldialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnModelGen, SIGNAL(clicked()), this, SLOT(onModelGenClicked()));
    connect(ui->btnDataGen, SIGNAL(clicked()), this, SLOT(onDatabaseGenClicked()));
    connect(ui->btnRestoreModel, SIGNAL(clicked()), this, SLOT(onRestoreClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onModelGenClicked(){
    (new GenerateModelParamsDialog(this))->exec();
}

void MainWindow::onDatabaseGenClicked(){
    (new GenerateDataDialog(this))->exec();
}

void MainWindow::onRestoreClicked(){
    (new RestoreModelDialog(this))->exec();
}
