#include <QDir>

#include "MainWindow.h"
#include "ui_MainWindow.h"
#include "MainWindowlog.h"
#include "Util/UrlProcess.h"
#include "BiliApi/BilibiliClient.h"
#include "Sqlite/SQLiteManager.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    SetUi();

    setWindowFlags(windowFlags() | Qt::FramelessWindowHint);
    setWindowTitle(tr("Bili Downloader"));
    SignalsAndSlots();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::SearchUrl()
{



}

void MainWindow::SetUi()
{
    installEventFilter(ui->widgetTitle);
}

void MainWindow::SignalsAndSlots()
{

}
