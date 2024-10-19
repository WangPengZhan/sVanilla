#include "mainwindow.h"
#include "ui_mainwindow.h"

#include "FreezeModel.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setUi();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::setUi()
{
    auto model = new FreezeModel();
    ui->tableView->setModel(model);
    ui->tableView->setFrozenColumnCount(3);
}
