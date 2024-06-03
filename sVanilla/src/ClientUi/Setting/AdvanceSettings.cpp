#include <QButtonGroup>

#include "AdvanceSettings.h"
#include "ui_AdvanceSettings.h"
#include "ClientUi/Config/SingleConfig.h"

AdvanceSettings::AdvanceSettings(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AdvanceSettings)
    , m_videoOrderGroup(new QButtonGroup(this))
    , m_videoDefaultGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

AdvanceSettings::~AdvanceSettings()
{
    delete ui;
}

void AdvanceSettings::setUi()
{
    m_videoOrderGroup->addButton(ui->cbxAsc, 0);
    m_videoOrderGroup->addButton(ui->cbxDes, 1);
    m_videoDefaultGroup->addButton(ui->cbxGrid, 0);
    m_videoDefaultGroup->addButton(ui->cbxList, 1);

    auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
    videoWidgetConfig.order ? ui->cbxDes->setChecked(true) : ui->cbxAsc->setChecked(true);
    int index = ui->comboBoxOrderElem->findText(videoWidgetConfig.orderBy);
    ui->comboBoxOrderElem->setCurrentIndex(index);
    ui->cbxNoParseList->setChecked(videoWidgetConfig.isNoParseList);
    videoWidgetConfig.widgetLayout ? ui->cbxList->setChecked(true) : ui->cbxGrid->setChecked(true);
}

void AdvanceSettings::signalsAndSlots()
{
    connect(ui->cbxAsc, &QCheckBox::clicked, this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.order = !ui->cbxAsc->isChecked();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
    connect(ui->cbxDes, &QCheckBox::clicked, this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.order = ui->cbxDes->isChecked();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
    connect(ui->comboBoxOrderElem, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.orderBy = ui->comboBoxOrderElem->currentText();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
    connect(ui->cbxNoParseList, &QCheckBox::clicked, this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.isNoParseList = ui->cbxNoParseList->isChecked();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
    connect(ui->cbxGrid, &QCheckBox::clicked, this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.widgetLayout = !ui->cbxGrid->isChecked();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
    connect(ui->cbxList, &QCheckBox::clicked, this, [&]() {
        auto videoWidgetConfig = SingleConfig::instance().videoWidgetConfig();
        videoWidgetConfig.widgetLayout = ui->cbxList->isChecked();
        SingleConfig::instance().setVideoWidgetConfig(videoWidgetConfig);
    });
}
