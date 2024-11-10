#include <QString>
#include <QStringList>
#include <QRegularExpression>

#include "UrlProcess.h"

namespace util
{

bool UrlProcess::IsUrl(const QString& text)
{
    QRegularExpression url("^((http|https|ftp)://|magnet:\\?\\S+)$", QRegularExpression::CaseInsensitiveOption);
    return url.match(text).hasMatch();
    //    return text.startsWith("http://") || text.startsWith("https://");
}

bool UrlProcess::IsIntId(const QString& text)
{
    bool bIsInt = false;
    text.toInt(&bIsInt);
    return bIsInt;
}

bool UrlProcess::IsBvId(const QString& text)
{
    return text.startsWith("BV") && text.length() == 12;
}

bool UrlProcess::IsBvUrl(const QString& text)
{
    return IsBvId(GetVideoId(text));
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

std::string u8ToString(const std::u8string& u8Str)
{
    return std::string(reinterpret_cast<const char*>(u8Str.data()), u8Str.size());
}

std::string urlDecode(const std::string& encoded)
{
    std::string decoded;
    for (size_t i = 0; i < encoded.length(); ++i)
    {
        if (encoded[i] == '%')
        {
            if (i + 2 < encoded.length())
            {
                std::string hex = encoded.substr(i + 1, 2);
                char decodedChar = static_cast<char>(std::stoi(hex, nullptr, 16));
                decoded += decodedChar;
                i += 2;  // Skip the next two characters
            }
        }
        else
        {
            decoded += encoded[i];
        }
    }
    return decoded;
}

QString FileHelp::removeSpecialChar(const QString& path)
{
    QString temp = path;
    for (const auto& splicalChar : QString(specialChars))
    {
        temp.remove(splicalChar);
    }

    return temp;
}

std::string FileHelp::removeSpecialChar(const std::string& path)
{
    QString qPath = QString::fromStdString(path);
    return removeSpecialChar(qPath).toStdString();
}

}  // namespace util
