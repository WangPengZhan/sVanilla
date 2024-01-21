#include <sstream>
#include <QButtonGroup>
#include "DefaultSetting.h"
#include "ui_DefaultSetting.h"
#include "ClientUi/Event.h"

DefaultSetting::DefaultSetting(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DefaultSetting)
    , m_themeGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    m_themeGroup->addButton(ui->LightThemeButton, 0);
    m_themeGroup->addButton(ui->DarkThemeButton, 1);
    m_themeGroup->addButton(ui->AutoThemeButton, 2);
    SignalsAndSlots();
}

DefaultSetting::~DefaultSetting()
{
    delete ui;
}
void DefaultSetting::SignalsAndSlots()
{
    //   update  request information to  version label (core -> ui)
    connect(Event::getInstance(), &Event::updateAria2Version, this, [this](std::shared_ptr<aria2net::AriaVersion> version) {
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
    });

    // transfer theme btn click signal to core to update theme(ui -> core)
    connect(m_themeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton* button) {
        const int id = m_themeGroup->id(button);
        emit UpdateTheme(id);
    });
}
void DefaultSetting::updateStatus(const std::string& status)
{
    ui->Aria2Status->setText(QString::fromStdString(status));
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
    ui->Aria2Status->setStyleSheet("color: rgb(191,49,49);");
}
void DefaultSetting::setGreenStatus()
{
    ui->Aria2Status->setStyleSheet("color: rgb(115,144,114);");
}
