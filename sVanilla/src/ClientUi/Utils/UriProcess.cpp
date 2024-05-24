
#include "UriProcess.h"

#include "BiliApi/BilibiliUrl.h"
#include "ClientUi/MainWindow/SApplication.h"
#include "Util/HistoryUtil.h"

#include <QtCore/qfile.h>

UriProcess::UriProcess(QObject* parent)
    : QObject(parent)
{
}

void UriProcess::parseUri(const std::string& uri)
{
    const auto type = checkUriType(uri);
    if (type.empty())
    {
        return;
    }
    UriInfo info;
    info.type = type;
    if (type == "bilibili")
    {
        const auto id = getID(uri);
        if (!id.empty())
        {
            info.id = id;
            emit uriProcessComplete(info);
        }
    }
    else if (type == "default")
    {
        info.id = uri;
        emit uriProcessComplete(info);
    }
    SearchHistory::global().append(uri);
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

    // will load plugin for check, return plugin name
    if (isValidUrl(uri))
    {
        return "bilibili";
    }
    return "default";
}