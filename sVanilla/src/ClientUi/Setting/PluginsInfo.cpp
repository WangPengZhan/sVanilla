#include <QMessageBox>

#include "PluginsInfo.h"
#include "ui_PluginsInfo.h"
#include "MainWindow/SApplication.h"
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

void PluginsInfo::showEvent(QShowEvent* event)
{
    if (m_showFirst)
    {
        ui->tableWidget->setColumnWidth(0, 160);
        ui->tableWidget->setColumnWidth(1, 48);
        ui->tableWidget->setColumnWidth(2, 60);
        ui->tableWidget->setColumnWidth(3, 48);
        ui->tableWidget->setColumnWidth(4, 240);
        m_showFirst = false;
    }

    initPluginInfos();

    return QWidget::showEvent(event);
}

void PluginsInfo::setUi()
{
    auto horHeader = ui->tableWidget->horizontalHeader();
    horHeader->setDefaultAlignment(Qt::AlignLeft);
}

void PluginsInfo::signalsAndSlots()
{
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &PluginsInfo::setPluginConfig);
}

void PluginsInfo::initPluginInfos()
{
    auto pluginsInfo = sApp->pluginManager().getPluginConfig();
    disconnect(ui->tableWidget, &QTableWidget::itemChanged, this, &PluginsInfo::setPluginConfig);
    ui->tableWidget->setRowCount(pluginsInfo.size());
    int i = 0;
    for (const auto& pluginInfo : pluginsInfo)
    {
        int col = 0;
        ui->tableWidget->setItem(i, col++, new QTableWidgetItem(QString::fromStdString(pluginInfo.name)));
        auto item = new QTableWidgetItem;
        item->setCheckState(pluginInfo.enabled ? Qt::Checked : Qt::Unchecked);
        ui->tableWidget->setItem(i, col++, item);
        ui->tableWidget->setItem(i, col++, new QTableWidgetItem(QString::fromStdString(pluginInfo.version)));
        ui->tableWidget->setItem(i, col++, new QTableWidgetItem(QString::number(pluginInfo.id)));
        ui->tableWidget->setItem(i, col++, new QTableWidgetItem(QString::fromStdString(pluginInfo.libFile)));
        ui->tableWidget->setItem(i, col++, new QTableWidgetItem(QString::fromStdString(pluginInfo.description)));

        if (!pluginInfo.isValid)
        {
            int cols = ui->tableWidget->columnCount();
            for (int j = 0; j < cols; ++j)
            {
                auto flags = ui->tableWidget->item(i, j)->flags();
                ui->tableWidget->item(i, j)->setFlags(flags & ~Qt::ItemIsEnabled);
            }
        }

        i++;
    }
    connect(ui->tableWidget, &QTableWidget::itemChanged, this, &PluginsInfo::setPluginConfig);
}

void PluginsInfo::setPluginConfig()
{
    if (m_isTip)
    {
        QMessageBox::information(this, tr("Info"), tr("It takes effect only after the restart"), QMessageBox::Ok);
        m_isTip = false;
    }

    int rows = ui->tableWidget->rowCount();

    std::vector<plugin::PluginConfig> pluginConfig;
    pluginConfig.reserve(rows);
    for (int i = 0; i < rows; ++i)
    {
        plugin::PluginConfig pluginInfo;
        int col = 0;
        int colMax = ui->tableWidget->colorCount() - 1;
        pluginInfo.name = ui->tableWidget->item(i, col++)->text().toStdString();
        pluginInfo.enabled = ui->tableWidget->item(i, col++)->checkState() == Qt::Unchecked;
        pluginInfo.version = ui->tableWidget->item(i, col++)->text().toStdString();
        pluginInfo.id = ui->tableWidget->item(i, col++)->text().toInt();
        pluginInfo.libFile = ui->tableWidget->item(i, col++)->text().toStdString();
        pluginInfo.description = ui->tableWidget->item(i, col++)->text().toStdString();
        pluginInfo.isValid = ui->tableWidget->item(i, colMax)->flags() & Qt::ItemIsEnabled;
        pluginConfig.emplace_back(std::move(pluginInfo));
    }

    sApp->pluginManager().setPluginConfig(pluginConfig);
}
