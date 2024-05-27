#include "PluginsInfo.h"
#include "ui_PluginsInfo.h"
#include "ClientUi//MainWindow/SApplication.h"
#include "Plugin/PluginManager.h"

PluginsInfo::PluginsInfo(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::PluginsInfo)
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
    initPluginInfos();
}

PluginsInfo::~PluginsInfo()
{
    delete ui;
}

void PluginsInfo::setUi()
{
    auto horHeader = ui->tableWidget->horizontalHeader();
    horHeader->setDefaultAlignment(Qt::AlignLeft);
}

void PluginsInfo::signalsAndSlots()
{
}

void PluginsInfo::initPluginInfos()
{
    auto pluginsInfo = sApp->pluginManager().getPluginsInfo();
    ui->tableWidget->setRowCount(pluginsInfo.size());
    int i = 0;
    for (const auto& pluginInfo : pluginsInfo)
    {
        ui->tableWidget->setItem(i, 0, new QTableWidgetItem(QString::fromStdString(pluginInfo.name)));
        ui->tableWidget->setItem(i, 1, new QTableWidgetItem(QString::fromStdString(pluginInfo.version)));
        ui->tableWidget->setItem(i, 2, new QTableWidgetItem(QString::number(pluginInfo.id)));
        ui->tableWidget->setItem(i, 3, new QTableWidgetItem(QString::fromStdString(pluginInfo.libFile)));
        ui->tableWidget->setItem(i, 4, new QTableWidgetItem(QString::fromStdString(pluginInfo.description)));
        i++;
    }
}
