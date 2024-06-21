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
    bool enableAdvancedSetting = false;
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
    QString nameRule = "$title$";
};

struct SystemTrayConfig
{
    bool enable = true;
    bool minimize = false;
};

struct StartUpConfig
{
    bool autoStart = false;
    bool keepMainWindow = false;
    bool autoRemuseUnfinishedTask = false;
};

struct VideoWidgetConfig
{
    int order{0};
    QString orderBy;
    bool isNoParseList = false;
    int widgetLayout{0};
};

class SingleConfig
{
public:
    static SingleConfig& instance();

    void setAriaConfig(const Aria2Config& config);
    Aria2Config ariaConfig() const;

    const std::shared_ptr<QSettings>& aria2AdvanceConfig() const;

    void setDownloadConfig(const DownloadConfig& config);
    DownloadConfig downloadConfig() const;

    void setTheme(int theme);
    int theme() const;

    void setLanguage(int language);
    int language() const;

    void setSystemTrayConfig(const SystemTrayConfig& systemTrayConfig);
    SystemTrayConfig systemTrayConfig() const;

    void setStartUpConfig(const StartUpConfig& startUpConfig);
    StartUpConfig startConfig() const;

    void setDownloadThreadNum(int threadNum);
    int downloadThreadNum() const;

    void setVideoWidgetConfig(const VideoWidgetConfig& startUpConfig);
    VideoWidgetConfig videoWidgetConfig() const;

private:
    SingleConfig();
    ~SingleConfig();

    void iniConfig();

private:
    std::shared_ptr<QSettings> m_appSettings;
    std::shared_ptr<QSettings> m_aria2Settings;
    std::shared_ptr<QSettings> m_aria2DefaultSettings;
    std::shared_ptr<QSettings> m_aria2CustomSettings;
};
