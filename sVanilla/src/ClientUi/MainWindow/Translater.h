#pragma once
#include <QObject>
#include <QString>
#include <QTranslator>

class Translater : public QObject
{
public:
    enum Language
    {
        English,
        Chinese,
        ChineseTw,
        Unknow = 255,
    };
    using QObject::QObject;
    Translater(QString translatesDir, QObject* parent);

    void setTranslatesDir(const QString& translatesDir);
    const QString& translatesDir() const;

    QString fileDir(Language Language) const;

    void setLanguage(Language Language);
    Language lanuage() const;

private:
    QString m_translatesDir;
    Language m_language = Unknow;
    std::vector<std::shared_ptr<QTranslator>> m_translators;
};