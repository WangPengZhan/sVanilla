#include "UrlProcess.h"

#include <QString>
#include <QStringList>

namespace util
{

bool UrlProcess::IsUrl(const QString& text)
{
    return text.startsWith("http://") || text.startsWith("https://");
}

bool UrlProcess::IsIntId(const QString& text)
{
    bool bIsInt = false;
    text.toInt(&bIsInt);
    return bIsInt
   ;
}

bool UrlProcess::IsBvId(const QString& text)
{
    return text.startsWith("BV") && text.length() == 12;
}

bool UrlProcess::IsBvUrl(const QString& text)
{
    return IsBvId(GetVideoId(text));;
}

QString UrlProcess::DeleteUrlParam(const QString& url)
{
    QStringList strList = url.split("?");
    return strList[0].endsWith("/") ? strList[0].remove(strList[0].size() - 1, 1) : strList[0];
}

QString UrlProcess::GetId(const QString& url, const QString& baseUrl)
{
    if (!IsUrl(url))
    {
        return {};
    }

    QString tempUrl = EnableHttps(url);
    tempUrl = DeleteUrlParam(tempUrl);

    tempUrl = tempUrl.replace(QString::fromLocal8Bit(ShareWwwUrl.data()), QString::fromLocal8Bit(WwwUrl.data()));
    tempUrl = tempUrl.replace(QString::fromLocal8Bit(MobileUrl.data()), QString::fromLocal8Bit(WwwUrl.data()));

    if (tempUrl.contains("b23.tv/ss") || tempUrl.contains("b23.tv/ep"))
    {
        tempUrl = tempUrl.replace(QString::fromLocal8Bit(ShortUrl.data()), QString::fromLocal8Bit(BangumiUrl.data()));
    }
    else
    {
        tempUrl = tempUrl.replace(QString::fromLocal8Bit(ShortUrl.data()), QString::fromLocal8Bit(VideoUrl.data()));
    }

    if (!tempUrl.startsWith(baseUrl))
    {
        return {};
    }

    return tempUrl.replace(baseUrl, "");
}

QString UrlProcess::GetVideoId(const QString& text)
{
    return GetId(text, QString::fromLocal8Bit(VideoUrl.data()));
}

QString UrlProcess::EnableHttps(const QString& url)
{
    QString result = url;
    if (IsUrl(url))
    {
        return result.replace("http://", "https://");
    }
    return {};
}

QString FileHelp::RemoveSpicalChar(const QString& path)
{
    QString temp = path;
    for (const auto& splicalChar : QString(splicalChars))
    {
        temp.remove(splicalChar);
    }

    return temp;
}

} // util


