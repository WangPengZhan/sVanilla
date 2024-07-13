#include <sstream>

#include <QButtonGroup>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QSettings>

#include "DefaultSettings.h"
#include "ui_DefaultSettings.h"
#include "Config/SingleConfig.h"
#include "MainWindow/SApplication.h"
#include "VideoList/VideoData.h"

DefaultSettings::DefaultSettings(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DefaultSettings)
    , m_themeGroup(new QButtonGroup(this))
{
    ui->setupUi(this);
    setUi();
    loadConfigToUi();
    signalsAndSlots();
    emit updateTheme(m_themeGroup->checkedId());
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

    ui->widgetNameRule->hidePreviewLabel(true);

    std::unordered_map<std::string, std::string> rules;
    for (const auto& rule : VideoInfoFull::ruleList)
    {
        rules.insert({rule, ""});
    }
    ui->widgetNameRule->init(rules);
}

void DefaultSettings::signalsAndSlots()
{
    connect(m_themeGroup, static_cast<void (QButtonGroup::*)(QAbstractButton*)>(&QButtonGroup::buttonClicked), this, [this](QAbstractButton* button) {
        const int id = m_themeGroup->id(button);
        emit updateTheme(id);
        SingleConfig::instance().setTheme(id);
    });

    connect(ui->comboBoxLanguge, static_cast<void (QComboBox::*)(int)>(&QComboBox::currentIndexChanged), this, [this](int language) {
        sApp->translater().setLanguage(static_cast<Translater::Language>(language));
    });

    connect(ui->checkBoxEnableTray, &QCheckBox::stateChanged, this, [&](bool enable) {
        emit DefaultSettings::enableTray(enable);
        SystemTrayConfig systemTrayConfig = SingleConfig::instance().systemTrayConfig();
        systemTrayConfig.enable = enable;
        SingleConfig::instance().setSystemTrayConfig(systemTrayConfig);
    });

    connect(ui->checkBoxMinToTray, &QCheckBox::stateChanged, this, [this](int state) {
        SystemTrayConfig systemTrayConfig = SingleConfig::instance().systemTrayConfig();
        systemTrayConfig.minimize = state;
        SingleConfig::instance().setSystemTrayConfig(systemTrayConfig);
    });

    connect(ui->checkBoxMinToTray, &QCheckBox::stateChanged, this, [this](int state) {
        SystemTrayConfig systemTrayConfig = SingleConfig::instance().systemTrayConfig();
        systemTrayConfig.minimize = state;
        SingleConfig::instance().setSystemTrayConfig(systemTrayConfig);
    });

    connect(ui->checkBoxOpenStartup, &QCheckBox::stateChanged, this, [this](int state) {
        autoStartRun(state);
        auto startConfig = SingleConfig::instance().startConfig();
        startConfig.autoStart = state;
        SingleConfig::instance().setStartUpConfig(startConfig);
    });

    connect(ui->checkBoxKeepState, &QCheckBox::stateChanged, this, [this](int state) {
        auto startConfig = SingleConfig::instance().startConfig();
        startConfig.keepMainWindow = state;
        SingleConfig::instance().setStartUpConfig(startConfig);
    });

    connect(ui->checkBoxResumeTask, &QCheckBox::stateChanged, this, [this](int state) {
        auto startConfig = SingleConfig::instance().startConfig();
        startConfig.autoRemuseUnfinishedTask = state;
        SingleConfig::instance().setStartUpConfig(startConfig);
    });

    connect(ui->folderSelectorDownloadPath, &QLineEdit::textChanged, this, [this](const QString& newText) {
        auto downloadConfig = SingleConfig::instance().downloadConfig();
        downloadConfig.downloadDir = newText;
        SingleConfig::instance().setDownloadConfig(downloadConfig);
    });

    connect(ui->widgetNameRule, &NameRuleWidget::editingFinished, this, [this](const QString& newText) {
        auto downloadConfig = SingleConfig::instance().downloadConfig();
        downloadConfig.nameRule = newText;
        SingleConfig::instance().setDownloadConfig(downloadConfig);
    });
}

void DefaultSettings::loadConfigToUi()
{
    auto& singleConfig = SingleConfig::instance();
    auto theme = singleConfig.theme();
    if (theme == 0)
    {
        ui->LightThemeButton->setChecked(true);
    }
    else if (theme == 1)
    {
        ui->DarkThemeButton->setChecked(true);
    }
    else if (theme == 2)
    {
        ui->AutoThemeButton->setChecked(true);
    }

    ui->comboBoxLanguge->setCurrentIndex(singleConfig.language());
    ui->checkBoxEnableTray->setCheckState(Qt::CheckState::Checked);
    auto systemTrayConfig = singleConfig.systemTrayConfig();
    ui->checkBoxEnableTray->setCheckState(systemTrayConfig.enable ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkBoxMinToTray->setCheckState(systemTrayConfig.minimize ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    auto startConfig = singleConfig.startConfig();
    ui->checkBoxOpenStartup->setCheckState(startConfig.autoStart ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkBoxKeepState->setCheckState(startConfig.keepMainWindow ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->checkBoxResumeTask->setCheckState(startConfig.autoRemuseUnfinishedTask ? Qt::CheckState::Checked : Qt::CheckState::Unchecked);
    ui->spinBoxThread->setValue(singleConfig.downloadThreadNum());

    auto downloadConfig = singleConfig.downloadConfig();
    ui->folderSelectorDownloadPath->setText(downloadConfig.downloadDir);
    ui->widgetNameRule->updateLineEdit(downloadConfig.nameRule);
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

Qt::CheckState DefaultSettings::isSaveMainWindow() const
{
    return ui->checkBoxKeepState->checkState();
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

void DefaultSettings::autoStartRun(bool isRun)
{
    QString appName = QCoreApplication::applicationName();
    QString appPath = QCoreApplication::applicationFilePath();

#ifdef _WIN32
    QSettings settings("HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);
    if (isRun)
    {
        settings.setValue(appName, appPath.replace("/", "\\"));
    }
    else
    {
        settings.remove(appName);
    }
#elif __APPLE__
    QString appListName = "sVanilla.my.app.plist";
    QString appList = QDir::cleanPath(QStandardPaths::writableLocation(QStandardPaths::HomeLocation) + QDir::separator() + "Library" + QDir::separator() +
                                      "LaunchAgents" + QDir::separator() + appListName);

    QSettings autoRun(appPath, QSettings::NativeFormat);
    autoRun.setValue("Label", "sVanilla.my.app.plist");
    autoRun.setValue("Program", appPath);
    autoRun.setValue("WorkingDirectory", QDir::cleanPath(QCoreApplication::applicationDirPath()));
    autoRun.setValue("Disabled", false);
    autoRun.setValue("RunAtLoad", isRun);
#elif __linux__
    std::string appDesktopFile = QCoreApplication::applicationDirPath().toStdString() + "/share/applications/sVanilla.desktop";
    std::string autoCopyPath = "/etc/xdg/autostart/sVanilla.desktop";
    std::string cmd;
    if (isRun)
    {
        cmd = "pkexec cp " + appDesktopFile + " " + autoCopyPath;
    }
    else
    {
        cmd = "pkexec rm " + autoCopyPath;
    }

    auto res = system(cmd.c_str());
#endif
}
