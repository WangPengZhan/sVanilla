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

enum class VideQuality
{
    Best = -1,  // auto
    R240P,
    R360P,
    R480P,
    R720P,
    R720P60F,
    R1080P,
    R1080PL,
    R1080R60F,
    R2160P,
};

struct DownloadConfig
{
    QString downloadDir;
    VideQuality videoQuality = VideQuality::Best;
    QString nameRule = "$title$-$author$";
};

class SingleConfig
{
public:
    static SingleConfig& instance();

    void setAriaConfig(const Aria2Config& config);
    Aria2Config ariaConfig() const;

    void setDownloadConfig(const DownloadConfig& config);
    DownloadConfig downloadConfig() const;

private:
    SingleConfig();
    ~SingleConfig();

    void iniConfig();

private:
    std::shared_ptr<QSettings> m_appSettings;
    std::shared_ptr<QSettings> m_aria2Settings;
};
