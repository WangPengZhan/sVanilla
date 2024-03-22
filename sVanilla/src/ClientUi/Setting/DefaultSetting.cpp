#include <sstream>

#include <QButtonGroup>

#include "DefaultSetting.h"
#include "ui_DefaultSetting.h"

DefaultSetting::DefaultSetting(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DefaultSetting)
    , m_themeGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    m_themeGroup->addButton(ui->LightThemeButton, 0);
    m_themeGroup->addButton(ui->DarkThemeButton, 1);
    m_themeGroup->addButton(ui->AutoThemeButton, 2);
    signalsAndSlots();
}

DefaultSetting::~DefaultSetting()
{
    delete ui;
}
void DefaultSetting::updateAria2Version(const std::shared_ptr<aria2net::AriaVersion>& version)
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
            ss << str << "<br>";
        }
        updateFeatures(ss.str());
    }
}
void DefaultSetting::signalsAndSlots()
{
    connect(m_themeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton* button) {
        const int id = m_themeGroup->id(button);
        emit UpdateTheme(id);
    });
}
void DefaultSetting::updateStatus(const std::string& status)
{
    ui->Aria2Status->setText(QString::fromStdString(status));
    // ui->Aria2Status->setPalette(QColor(255, 255, 255));
}
void DefaultSetting::updateVersion(const std::string& version)
{
    ui->Aria2Version->setText(QString::fromStdString(version));
}
void DefaultSetting::updateFeatures(const std::string& features)
{
    ui->Aria2Features->setText(QString::fromStdString(features));
}
void DefaultSetting::setRedStatus()
{
    auto palette = ui->Aria2Status->palette();
    palette.setColor(QPalette::WindowText, QColor(191, 49, 49));
    ui->Aria2Status->setPalette(palette);
}
void DefaultSetting::setGreenStatus()
{
    auto palette = ui->Aria2Status->palette();
    palette.setColor(QPalette::WindowText, QColor(115, 144, 114));
    ui->Aria2Status->setPalette(palette);
}
