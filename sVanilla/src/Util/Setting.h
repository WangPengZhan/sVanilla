#pragma once
#include <QSettings>
#include <QFileInfo>
#include <shared_mutex>

class CustomSettings
{
public:
    explicit CustomSettings(QSettings* settings);
    auto setConfFormat() const -> void;
    void writeSetting(const QString& group, const QString& key, const QVariant& value);
    QVariant readSetting(const QString& group, const QString& key);
    void clear();

private:
    QSettings* m_settings;
    std::shared_mutex m_shared_mutex;
    const QSettings::Format ConfFormat = QSettings::registerFormat("conf", readConfFile, writeConfFile);
    static bool readConfFile(QIODevice& device, QSettings::SettingsMap& map);
    static bool writeConfFile(QIODevice& device, const QSettings::SettingsMap& map);
};

class Settings
{
public:
    enum Format
    {
        IniFormat,
        ConfFormat
    };
    explicit Settings(QString path, Format format);
    void write(const QString& group, const QString& key, const QVariant& value) const;
    [[nodiscard]] QVariant read(const QString& group, const QString& key) const;
    [[nodiscard]] std::string readString(const QString& group, const QString& key) const;
    void clear() const;

private:
    QString m_path;
    std::shared_ptr<CustomSettings> customSettings;
};