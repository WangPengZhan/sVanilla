#include <QApplication>
#include <QFile>
#include <QSettings>
#include <QStandardPaths>
#include "ClientUi/MainWindow/SApplication.h"

#include "Utils/Setting.h"
#include "SingleConfig.h"

#define CONFIG_REMOTE 0

SingleConfig& SingleConfig::instance()
{
    static SingleConfig instance;
    return instance;
}

void SingleConfig::setAriaConfig(const Aria2Config& config)
{
    m_appSettings->beginGroup("App");
    m_appSettings->setValue("Url", config.url);
    m_appSettings->setValue("Token", config.token);
    m_appSettings->setValue("Port", config.port);
    m_appSettings->setValue("Remote", config.isRemote);
    m_appSettings->setValue("EnableAdvancedSetting", config.enableAdvancedSetting);
    m_appSettings->endGroup();
}

Aria2Config SingleConfig::ariaConfig() const
{
    Aria2Config config;
    m_appSettings->beginGroup("App");
    config.url = m_appSettings->value("Url").toString();
    config.token = m_appSettings->value("Token").toString();
    config.port = m_appSettings->value("Port").toInt();
    config.isRemote = m_appSettings->value("Remote").toBool();
    config.enableAdvancedSetting = m_appSettings->value("EnableAdvancedSetting").toBool();
    m_appSettings->endGroup();
    return config;
}

const std::shared_ptr<QSettings>& SingleConfig::aria2AdvanceConfig() const
{
    return m_aria2CustomSettings;
}

void SingleConfig::setDownloadConfig(const DownloadConfig& config)
{
    m_appSettings->beginGroup("Download");
    m_appSettings->setValue("DownloadDir", config.downloadDir);
    m_appSettings->setValue("VideoQuality", static_cast<int>(config.videoQuality));
    m_appSettings->setValue("NameRule", config.nameRule);
    m_appSettings->endGroup();
}

DownloadConfig SingleConfig::downloadConfig() const
{
    DownloadConfig config;
    m_appSettings->beginGroup("Download");
    config.downloadDir = m_appSettings->value("DownloadDir").toString();
    config.videoQuality = static_cast<VideQuality>(m_appSettings->value("VideoQuality").toInt());
    config.nameRule = m_appSettings->value("NameRule").toString();
    m_appSettings->endGroup();
    return config;
}

void SingleConfig::setTheme(int theme)
{
    m_appSettings->beginGroup("AppConfig");
    m_appSettings->setValue("Theme", theme);
    m_appSettings->endGroup();
}

int SingleConfig::theme() const
{
    m_appSettings->beginGroup("AppConfig");
    auto nTheme = m_appSettings->value("Theme").toInt();
    m_appSettings->endGroup();
    return nTheme;
}

void SingleConfig::setLanguage(int language)
{
    m_appSettings->beginGroup("AppConfig");
    m_appSettings->setValue("Language", language);
    m_appSettings->endGroup();
}

int SingleConfig::language() const
{
    m_appSettings->beginGroup("AppConfig");
    int nLanguage = m_appSettings->value("Language").toInt();
    m_appSettings->endGroup();
    return nLanguage;
}

void SingleConfig::setSystemTrayConfig(const SystemTrayConfig& systemTrayConfig)
{
    m_appSettings->beginGroup("SystemTrayConfig");
    m_appSettings->setValue("Enable", systemTrayConfig.enable);
    m_appSettings->setValue("Minimize", systemTrayConfig.minimize);
    m_appSettings->endGroup();
}

SystemTrayConfig SingleConfig::systemTrayConfig() const
{
    SystemTrayConfig systemTrayConfig;
    m_appSettings->beginGroup("SystemTrayConfig");
    systemTrayConfig.enable = m_appSettings->value("Enable").toBool();
    systemTrayConfig.minimize = m_appSettings->value("Minimize").toBool();
    m_appSettings->endGroup();

    return systemTrayConfig;
}

void SingleConfig::setStartUpConfig(const StartUpConfig& startUpConfig)
{
    m_appSettings->beginGroup("StartUpConfig");
    m_appSettings->setValue("AutoStart", startUpConfig.autoStart);
    m_appSettings->setValue("KeepMainWindow", startUpConfig.keepMainWindow);
    m_appSettings->setValue("AutoRemuseUnfinishedTask", startUpConfig.autoRemuseUnfinishedTask);
    m_appSettings->endGroup();
}

StartUpConfig SingleConfig::startConfig() const
{
    StartUpConfig startUpConfig;
    m_appSettings->beginGroup("StartUpConfig");
    startUpConfig.autoStart = m_appSettings->value("AutoStart").toBool();
    startUpConfig.keepMainWindow = m_appSettings->value("KeepMainWindow").toBool();
    startUpConfig.autoRemuseUnfinishedTask = m_appSettings->value("AutoRemuseUnfinishedTask").toBool();
    m_appSettings->endGroup();

    return startUpConfig;
}

void SingleConfig::setDownloadThreadNum(int threadNum)
{
    m_appSettings->beginGroup("AppConfig");
    m_appSettings->setValue("ThreadNum", threadNum);
    m_appSettings->endGroup();
}

int SingleConfig::downloadThreadNum() const
{
    m_appSettings->beginGroup("AppConfig");
    int nThreadNum = m_appSettings->value("ThreadNum").toInt();
    m_appSettings->endGroup();
    return nThreadNum;
}

void SingleConfig::setVideoWidgetConfig(const VideoWidgetConfig& videoWidgetConfig)
{
    m_appSettings->beginGroup("VideoWidget");
    m_appSettings->setValue("Order", videoWidgetConfig.order);
    m_appSettings->setValue("OrderBy", videoWidgetConfig.orderBy);
    m_appSettings->setValue("NoParseList", videoWidgetConfig.isNoParseList);
    m_appSettings->setValue("WidgetLayout", videoWidgetConfig.widgetLayout);
    m_appSettings->endGroup();
}

VideoWidgetConfig SingleConfig::videoWidgetConfig() const
{
    VideoWidgetConfig videoWidgetConfig;

    m_appSettings->beginGroup("VideoWidget");
    videoWidgetConfig.order = m_appSettings->value("Order").toInt();
    videoWidgetConfig.orderBy = m_appSettings->value("OrderBy").toString();
    videoWidgetConfig.isNoParseList = m_appSettings->value("NoParseList").toBool();
    videoWidgetConfig.widgetLayout = m_appSettings->value("WidgetLayout").toInt();
    m_appSettings->endGroup();

    return videoWidgetConfig;
}

SingleConfig::SingleConfig()
{
    iniConfig();
}

SingleConfig::~SingleConfig()
{
}

void SingleConfig::iniConfig()
{
    const QString appConfigPath = SApplication::appDir() + "/config/config.ini";
    if (QFile::exists(appConfigPath))
    {
        m_appSettings = std::make_shared<QSettings>(appConfigPath, QSettings::IniFormat);

        // compatibility
        auto groups = m_appSettings->childGroups();
        if (!groups.contains("Download"))
        {
            DownloadConfig downloadConfig;
            downloadConfig.downloadDir = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).front();
            setDownloadConfig(downloadConfig);
        }
    }
    else
    {
        m_appSettings = std::make_shared<QSettings>(appConfigPath, QSettings::IniFormat);

        // default config
        Aria2Config config;
#if CONFIG_REMOTE
        config.url = "http://10.0.2.122";
        config.token = "sVanilla";
        config.port = 6800;
        config.isRemote = true;
#else
        config.url = "http://localhost";
        config.token = "sVanilla";
        config.port = 6800;
        config.isRemote = false;
#endif
        setAriaConfig(config);

        SystemTrayConfig systemTrayConfig;
        setSystemTrayConfig(systemTrayConfig);
        setDownloadThreadNum(1);
        setStartUpConfig(StartUpConfig());

        DownloadConfig downloadConfig;
        downloadConfig.downloadDir = QStandardPaths::standardLocations(QStandardPaths::DownloadLocation).front();
        setDownloadConfig(downloadConfig);
    }

    const QString aria2ConfigPath = SApplication::appDir() + "/config/aria2.conf";
    if (QFile::exists(aria2ConfigPath))
    {
        m_aria2Settings = std::make_shared<QSettings>(aria2ConfigPath, CustomSettings::m_confFormat);

        // compatibility
    }

    // default config
    const QString defaultPath = ":/aria2.conf";
    m_aria2DefaultSettings = std::make_shared<QSettings>(defaultPath, CustomSettings::m_confFormat);
    const QString mergeConfig = SApplication::appDir() + "/config/mergeAria2Config.conf";
    m_aria2CustomSettings = std::make_shared<QSettings>(mergeConfig, CustomSettings::m_confFormat);
    if (m_aria2Settings != nullptr)
    {
        const auto keys = m_aria2Settings->allKeys();
        for (const auto& key : keys)
        {
            const auto value = m_aria2Settings->value(key);
            m_aria2CustomSettings->setValue(key, value);
        }
    }
}
