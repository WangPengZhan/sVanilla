#pragma once

#include <memory>

#include <QString>

class QSettings;

struct Aria2Config
{
    QString url;
    QString token;
    int port = -1;
    bool isRemote = false;
};

class SingleConfig
{
public:
    static SingleConfig& instance();

    void setAriaConfig(const Aria2Config& config);
    Aria2Config getAriaConfig() const;

private:
    SingleConfig();
    ~SingleConfig();

    void iniConfig();

private:
    std::shared_ptr<QSettings> m_appSettings;
    std::shared_ptr<QSettings> m_aria2Settings;
};
