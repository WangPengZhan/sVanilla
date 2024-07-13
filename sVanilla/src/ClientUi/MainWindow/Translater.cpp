#include <QDir>
#include <QApplication>
#include <QDebug>

#include "Translater.h"
#include "Config/SingleConfig.h"

Translater::Translater(QString translatesDir, QObject* parent)
    : QObject(parent)
    , m_translatesDir(std::move(translatesDir))
{
}

void Translater::setTranslatesDir(const QString& translatesDir)
{
    m_translatesDir = translatesDir;
}

const QString& Translater::translatesDir() const
{
    return m_translatesDir;
}

QString Translater::fileDir(Language language) const
{
    switch (language)
    {
    case Translater::English:
        return m_translatesDir + "/en";
        break;
    case Translater::Chinese:
        return m_translatesDir + "/zh_CN";
        break;
    case Translater::ChineseTw:
        return m_translatesDir + "/zh_Tw";
        break;
    default:
        break;
    }

    return m_translatesDir;
}

void Translater::setLanguage(Language language)
{
    if (m_language == language)
    {
        return;
    }

    QDir translateDir(fileDir(language));
    translateDir.setNameFilters(QStringList() << "*.qm");
    QStringList translateFiles = translateDir.entryList();

    std::vector<std::shared_ptr<QTranslator>> translators;
    for (const auto& translateFile : translateFiles)
    {
        auto translator = std::make_shared<QTranslator>();
        if (!translator->load(translateFile, translateDir.absolutePath()))
        {
            qDebug() << "translator load error, file: " << translateFile;
            continue;
        }
        translators.push_back(translator);
    }

    for (const auto& translator : m_translators)
    {
        qApp->removeTranslator(translator.get());
    }
    m_translators = translators;
    if (language != English)
    {
        for (const auto& translator : m_translators)
        {
            qApp->installTranslator(translator.get());
        }
    }

    m_language = language;
    SingleConfig::instance().setLanguage(language);
}

Translater::Language Translater::lanuage() const
{
    return m_language;
}
