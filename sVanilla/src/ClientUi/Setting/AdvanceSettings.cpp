#include <QButtonGroup>

#include "AdvanceSettings.h"
#include "ui_AdvanceSettings.h"
#include "Config/SingleConfig.h"

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
    index = (index == -1 ? 0 : index);
    ui->comboBoxOrderElem->setCurrentIndex(index);
    ui->cbxNoParseList->setChecked(videoWidgetConfig.isNoParseList);
    videoWidgetConfig.widgetLayout ? ui->cbxList->setChecked(true) : ui->cbxGrid->setChecked(true);

    const auto aria2Config = SingleConfig::instance().ariaConfig();
    ui->lineEditAria2Url->setText(aria2Config.url);
    ui->lineEditAria2Token->setText(aria2Config.token);
    ui->spinBoxAria2Port->setValue(aria2Config.port);
    ui->checkBoxAria2EnableRemote->setChecked(aria2Config.isRemote);

    setAria2SettingVisiable(ui->checkBoxAria2EnableRemote->isChecked());
    ui->checkBoxEnableAria2AdvancedSettings->setChecked(aria2Config.enableAdvancedSetting);
    ui->listWidgetAria2AdvancedSettings->setVisible(aria2Config.enableAdvancedSetting);
    ui->scrollArea->adjustSize();
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
    connect(ui->lineEditAria2Url, &QLineEdit::editingFinished, this, [this]() {
        auto aria2Setting = SingleConfig::instance().ariaConfig();
        aria2Setting.url = ui->lineEditAria2Url->text();
        SingleConfig::instance().setAriaConfig(aria2Setting);
    });
    connect(ui->lineEditAria2Token, &QLineEdit::editingFinished, this, [this]() {
        auto aria2Setting = SingleConfig::instance().ariaConfig();
        aria2Setting.token = ui->lineEditAria2Token->text();
        SingleConfig::instance().setAriaConfig(aria2Setting);
    });
    connect(ui->spinBoxAria2Port, &QSpinBox::textChanged, this, [this](const QString& text) {
        auto aria2Setting = SingleConfig::instance().ariaConfig();
        aria2Setting.port = std::stoi(text.toStdString());
        SingleConfig::instance().setAriaConfig(aria2Setting);
    });
    connect(ui->checkBoxAria2EnableRemote, &QCheckBox::clicked, this, [this]() {
        auto aria2Setting = SingleConfig::instance().ariaConfig();
        const auto isChecked = ui->checkBoxAria2EnableRemote->isChecked();
        aria2Setting.isRemote = isChecked;
        SingleConfig::instance().setAriaConfig(aria2Setting);
        setAria2SettingVisiable(isChecked);
    });

    connect(ui->checkBoxEnableAria2AdvancedSettings, &QCheckBox::clicked, this, [this]() {
        auto aria2Setting = SingleConfig::instance().ariaConfig();
        const auto isChecked = ui->checkBoxEnableAria2AdvancedSettings->isChecked();
        aria2Setting.enableAdvancedSetting = isChecked;
        SingleConfig::instance().setAriaConfig(aria2Setting);
        ui->listWidgetAria2AdvancedSettings->setVisible(isChecked);
    });
}

void AdvanceSettings::setAria2SettingVisiable(const bool visiable) const
{
    ui->labelAria2Url->setVisible(visiable);
    ui->lineEditAria2Url->setVisible(visiable);
    ui->labelAria2Port->setVisible(visiable);
    ui->spinBoxAria2Port->setVisible(visiable);
    ui->labelAria2Token->setVisible(visiable);
    ui->lineEditAria2Token->setVisible(visiable);
}
