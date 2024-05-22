#include <sstream>

#include <QButtonGroup>

#include "DefaultSettings.h"
#include "ui_DefaultSettings.h"

DefaultSettings::DefaultSettings(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DefaultSettings)
    , m_themeGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    setUi();
    signalsAndSlots();
}

DefaultSettings::~DefaultSettings()
{
    delete ui;
}

void DefaultSettings::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version)
{
    if (version->id.empty() || (!version->error.message.empty()))
    {
        updateFeatures("-");
        setRedStatus();
        if (!version->id.empty())
        {
            updateVersion(version->error.message);
        }
    }
    else
    {
        updateStatus("Connected");
        setGreenStatus();
        updateVersion(version->result.version);
        std::stringstream ss;
        for (const auto& str : version->result.enabledFeatures)
        {
            ss << str << " ";
        }
        updateFeatures(ss.str());
    }
}

void DefaultSettings::setUi()
{
    m_themeGroup->addButton(ui->LightThemeButton, 0);
    m_themeGroup->addButton(ui->DarkThemeButton, 1);
    m_themeGroup->addButton(ui->AutoThemeButton, 2);

    ui->checkBoxEnableTray->setCheckState(Qt::CheckState::Checked);
}

void DefaultSettings::signalsAndSlots()
{
    connect(m_themeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton* button) {
        const int id = m_themeGroup->id(button);
        emit updateTheme(id);
    });

    connect(ui->checkBoxEnableTray, &QCheckBox::checkStateChanged, this, &DefaultSettings::enableTray);

    connect(ui->checkBoxMinToTray, &QCheckBox::checkStateChanged, this, [this](Qt::CheckState state) {
        if (state == Qt::CheckState::Checked)
        {
            ui->checkBoxEnableTray->setCheckState(Qt::CheckState::Checked);
        }
    });
}

void DefaultSettings::updateStatus(const std::string& status)
{
    ui->labelAria2Status->setText(QString::fromStdString(status));
}

void DefaultSettings::updateVersion(const std::string& version)
{
    ui->labelAria2Version->setText(QString::fromStdString(version));
}

void DefaultSettings::updateFeatures(const std::string& features)
{
    ui->labelAria2Features->setText(QString::fromStdString(features));
}

Qt::CheckState DefaultSettings::getTrayState() const
{
    return ui->checkBoxEnableTray->checkState();
}

Qt::CheckState DefaultSettings::isEnableMinimizeTray() const
{
    return ui->checkBoxMinToTray->checkState();
}

void DefaultSettings::setRedStatus()
{
    auto palette = ui->labelAria2Status->palette();
    palette.setColor(QPalette::WindowText, Qt::red);
    ui->labelAria2Status->setPalette(palette);
}

void DefaultSettings::setGreenStatus()
{
    auto palette = ui->labelAria2Status->palette();
    palette.setColor(QPalette::WindowText, Qt::darkGreen);
    ui->labelAria2Status->setPalette(palette);
}
