#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "generatemodelparamsdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    connect(ui->btnModelGen, SIGNAL(clicked()), this, SLOT(onModelGenClicked()));
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::onModelGenClicked(){
    (new GenerateModelParamsDialog(this))->exec();
}
