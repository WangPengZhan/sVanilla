#include <QTextStream>
#include <QMapIterator>

#include "Setting.h"

QSettings::Format CustomSettings::m_confFormat = registerConfFormat();

CustomSettings::CustomSettings(QSettings* settings)
    : m_settings(settings)
{
}

void CustomSettings::writeSetting(const QString& group, const QString& key, const QVariant& value)
{
    std::unique_lock lock(m_shared_mutex);
    if (!group.isEmpty())
    {
        m_settings->beginGroup(group);
    }

    m_settings->setValue(key, value);

    if (!group.isEmpty())
    {
        m_settings->endGroup();
    }
}

QVariant CustomSettings::readSetting(const QString& group, const QString& key)
{
    std::unique_lock lock(m_shared_mutex);

    if (!group.isEmpty())
    {
        m_settings->beginGroup(group);
    }

    QVariant value = m_settings->value(key);

    if (!group.isEmpty())
    {
        m_settings->endGroup();
    }

    return value;
}

void CustomSettings::clear()
{
    std::unique_lock lock(m_shared_mutex);
    m_settings->clear();
}

QSettings::Format CustomSettings::registerConfFormat()
{
    return QSettings::registerFormat("conf", readConfFile, writeConfFile);
}

bool CustomSettings::readConfFile(QIODevice& device, QSettings::SettingsMap& map)
{
    QTextStream stream(&device);
    while (!stream.atEnd())
    {
        if (QString line = stream.readLine().trimmed(); !line.isEmpty() && !line.startsWith("#"))
        {
            QStringList parts;
            parts = line.split("=");
            if (parts.size() == 2)
            {
                QString key = parts[0].trimmed();
                QString value = parts[1].trimmed();
                map.insert(key, value);
            }
        }
    }
    return true;
}

bool CustomSettings::writeConfFile(QIODevice& device, const QSettings::SettingsMap& map)
{
    QTextStream stream(&device);
    QMapIterator i(map);
    while (i.hasNext())
    {
        i.next();
        QString line = i.key() + " = " + i.value().toString();
        stream << line << Qt::endl;
    }
    return true;
}

Settings::Settings(QString path, const Format format)
    : m_path(std::move(path))
{
    if (m_path.isEmpty())
    {
        return;
    }
    if (format == ConfFormat)
    {
        customSettings = std::make_shared<CustomSettings>(new QSettings(m_path));
    }
    else if (format == IniFormat)
    {
        customSettings = std::make_shared<CustomSettings>(new QSettings(m_path, QSettings::NativeFormat));
    }
}

void Settings::write(const QString& group, const QString& key, const QVariant& value) const
{
    customSettings->writeSetting(group, key, value);
}

QVariant Settings::read(const QString& group, const QString& key) const
{
    return customSettings->readSetting(group, key);
}

std::string Settings::readString(const QString& group, const QString& key) const
{
    return customSettings->readSetting(group, key).toString().toStdString();
}

void Settings::clear() const
{
    customSettings->clear();
}
