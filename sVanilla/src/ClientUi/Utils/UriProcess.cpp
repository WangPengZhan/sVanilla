#include <QFile>

#include "UriProcess.h"
#include "BiliApi/BilibiliUrl.h"
#include "ClientUi/Storage/StorageManager.h"
#include "ClientUi/Storage/SearchHistoryStorage.h"

UriProcess::UriProcess(QObject* parent)
    : QObject(parent)
{
}

bool UriProcess::isUri(const std::string& uri)
{
    static constexpr auto patternStr = "^(?:(?:https?|ftp):\\/\\/)?"
                                       "(?:\\S+(?::\\S*)?@)?(?:(?!(?:10|127)(?:\\.\\d{1,3}){3})"
                                       "(?!(?:169\\.254|192\\.168)(?:\\.\\d{1,3}){2})(?!172\\.(?:1[6-9]|2\\d|3[0-1])"
                                       "(?:\\.\\d{1,3}){2})(?:[1-9]\\d?|1\\d\\d|2[01]\\d|22[0-3])"
                                       "(?:\\.(?:1?\\d{1,2}|2[0-4]\\d|25[0-5])){2}"
                                       "(?:\\.(?:[1-9]\\d?|1\\d\\d|2[0-4]\\d|25[0-4]))|(?:(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)"
                                       "(?:\\.(?:[a-z\u00a1-\uffff0-9]-*)*[a-z\u00a1-\uffff0-9]+)*(?:\\.(?:[a-z\u00a1-\uffff]{2,}))\\.?)"
                                       "(?::\\d{2,5})?(?:[/?#]\\S*)?$";

    std::regex pattern(patternStr);
    return std::regex_search(uri, pattern);
}

void UriProcess::parseUri(const std::string& uri)
{
    const auto type = checkUriType(uri);
    if (type.empty())
    {
        return;
    }

    auto historyStorage = sqlite::StorageManager::intance().searchHistoryStorage();

    UriInfo info;
    info.type = type;
    if (type == "bilibili")
    {
        const auto id = getID(uri);
        if (!id.empty())
        {
            info.id = id;
            emit uriProcessComplete(info);
            historyStorage->insertOrUpdate(uri, 1);
        }
    }
    else if (type == "default")
    {
        info.id = uri;
        emit uriProcessComplete(info);
        historyStorage->insertOrUpdate(uri, 0);
    }
}

void UriProcess::updateWebsiteIcon(const std::string& uri)
{
    const auto type = checkUriType(uri);
    if (type.empty())
    {
        return;
    }
    const auto iconPath = QString::fromStdString(":icon/website/" + type + ".svg");
    if (QFile::exists(iconPath))
    {
        emit setWebsiteIcon(iconPath);
    }
}

std::string UriProcess::checkUriType(const std::string& uri)
{
    if (uri.empty())
    {
        return {};
    }
    if (!isUri(uri))
    {
        return {};
    }

    if (isValidUrl(uri))
    {
        return "bilibili";
    }
    return "default";
}
