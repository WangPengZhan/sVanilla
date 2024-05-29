#include <QDir>
#include <qapplication.h>

#include "Translater.h"
#include "ClientUi/Config/SingleConfig.h"

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
    QStringList qmFiles = translateDir.entryList();

    for (const auto& translateFile : qmFiles)
    {
        m_translator.load(translateFile);
    }

    qApp->removeTranslator(&m_translator);
    if (language != English)
    {
        qApp->installTranslator(&m_translator);
    }

    m_language = language;
    SingleConfig::instance().setLanguage(language);
}

Translater::Language Translater::lanuage() const
{
    return m_language;
}
