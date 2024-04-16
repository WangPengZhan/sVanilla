#include <QApplication>
#include <QFile>
#include <QSettings>

#include "Util/Setting.h"
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
    m_appSettings->endGroup();
}

Aria2Config SingleConfig::getAriaConfig() const
{
    Aria2Config config;
    m_appSettings->beginGroup("App");
    config.url = m_appSettings->value("Url").toString();
    config.token = m_appSettings->value("Token").toString();
    config.port = m_appSettings->value("Port").toInt();
    config.isRemote = m_appSettings->value("Remote").toBool();
    m_appSettings->endGroup();
    return config;
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
    const QString appConfigPath = QApplication::applicationDirPath() + "/config/config.ini";
    if (QFile::exists(appConfigPath))
    {
        m_appSettings = std::make_shared<QSettings>(appConfigPath, QSettings::IniFormat);

        // compatibility
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
    }

    const QString aria2ConfigPath = QApplication::applicationDirPath() + "/config/aria2conf.conf";
    if (QFile::exists(aria2ConfigPath))
    {
        m_aria2Settings = std::make_shared<QSettings>(aria2ConfigPath, CustomSettings::m_confFormat);

        // compatibility
    }
    else
    {
        m_aria2Settings = std::make_shared<QSettings>(aria2ConfigPath, CustomSettings::m_confFormat);
        // default config
    }
}
