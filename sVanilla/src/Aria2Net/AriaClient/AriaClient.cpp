#include <QUuid>

#include "AriaClient.h"
#include "Logger/Logger.h"

namespace aria2net
{

const std::string GetRpcUri(int listenPort)
{
    return QString("http://localhost:%1/jsonrpc").arg(listenPort).toStdString();
}

const std::string GetGuid()
{
    QUuid id = QUuid::createUuid();
    std::string strGuid = id.toString(QUuid::Id128).toStdString();
    return strGuid;
}

AriaClient& AriaClient::GetInstance()
{
    static AriaClient ariaClient;
    return ariaClient;
}

SystemListNotifications AriaClient::listNotificationsAsync()
{
    AriaSendData ariaSend;

    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.listNotifications";

    return GetResult<SystemListNotifications>(ariaSend);
}

SystemListMethods AriaClient::ListMethodsAsync()
{
    AriaSendData ariaSend;

    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.listMethods";

    return GetResult<SystemListMethods>(ariaSend);
}

std::list<SystemMulticall> AriaClient::MulticallAsync(const std::list<SystemMulticallMathod>& systemMulticallMathods)
{
    std::list<std::string> listMethod;
    for (const auto method : systemMulticallMathods)
    {
        listMethod.emplace_back(method.toString());
    }

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.multicall";
    ariaSend.params = listMethod;

    nlohmann::json jsonResult = GetResult<nlohmann::json>(ariaSend);
    std::list<SystemMulticall> result;
    for (const auto& sigleJson : jsonResult)
    {
        result.emplace_back(sigleJson);
    }

    return result;
}

AriaSaveSession AriaClient::SaveSessionAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.saveSession";
    ariaSend.params = listString;
    return GetResult<AriaSaveSession>(ariaSend);
}

AriaShutdown AriaClient::ForceShutdownAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.forceShutdown";
    ariaSend.params = listString;

    return GetResult<AriaShutdown>(ariaSend);
}

AriaShutdown AriaClient::ShutdownAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.shutdown";
    ariaSend.params = listString;

    return GetResult<AriaShutdown>(ariaSend);
}

AriaGetSessionInfo AriaClient::GetSessionInfoAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getSessionInfo";
    ariaSend.params = listString;

    return GetResult<AriaGetSessionInfo>(ariaSend);
}

AriaVersion AriaClient::GetAriaVersionAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getVersion";
    ariaSend.params = listString;

    return GetResult<AriaVersion>(ariaSend);
}

AriaRemove AriaClient::RemoveDownloadResultAsync(const std::string& gid)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.emplace_back(gid);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.removeDownloadResult";
    ariaSend.params = listString;

    return GetResult<AriaRemove>(ariaSend);
}

AriaRemove AriaClient::PurgeDownloadResultAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.purgeDownloadResult";
    ariaSend.params = listString;

    return GetResult<AriaRemove>(ariaSend);
}

AriaGetGlobalStat AriaClient::GetGlobalStatAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params = listString;

    return GetResult<AriaGetGlobalStat>(ariaSend);
}

AriaChangeOption AriaClient::ChangeGlobalOptionAsync(const ListString& option)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.insert(listString.end(), option.begin(), option.end());

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params = listString;

    return GetResult<AriaChangeOption>(ariaSend);
}

AriaGetOption AriaClient::GetGlobalOptionAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalOption";
    ariaSend.params = listString;

    return GetResult<AriaGetOption>(ariaSend);
}

AriaGetOption AriaClient::GetOptionAsync(const std::string& gid)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.emplace_back(gid);

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getOption";
    ariaSend.params = listString;

    return GetResult<AriaGetOption>(ariaSend);
}

AriaChangeUri AriaClient::ChangeUriAsync(const std::string& gid, int fileIndex, ListString delUris, ListString addUris, int position)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.emplace_back(gid);
    listString.emplace_back(std::to_string(fileIndex));
    listString.insert(listString.end(), delUris.begin(), delUris.end());
    listString.insert(listString.end(), addUris.begin(), addUris.end());

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.changePosition";
    ariaSend.params = listString;

    return GetResult<AriaChangeUri>(ariaSend);
}

AriaChangePosition AriaClient::ChangePositionAsync(const std::string& gid, int pos, HowChangePosition how)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.emplace_back(gid);
    listString.emplace_back(std::to_string(pos));
    listString.emplace_back(std::to_string(how));

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.changePosition";
    ariaSend.params = listString;

    return GetResult<AriaChangePosition>(ariaSend);
}

AriaAddUri AriaClient::AddUriAsync(ListString uris, AriaSendOption option, int position)
{
    nlohmann::json params;
    nlohmann::json listJson;
    listJson = uris;

    params[0] = std::string("token:") + TOKEN;
    params[1] = listJson;
    params[2] = option;

    std::list<std::string> listString;
    listString.emplace_back(params[0].dump());
    listString.emplace_back(params[1].dump());
    listString.emplace_back(params[2].dump());

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.addUri";
    ariaSend.params = listString;

    return GetResult<AriaAddUri>(ariaSend);
}

AriaTellStatus AriaClient::TellStatus(const std::string& gid)
{
    nlohmann::json params;

    params[0] = std::string("token:") + TOKEN;
    params[1] = gid;

    std::list<std::string> listString;
    listString.emplace_back(params[0].dump());
    listString.emplace_back(params[1].dump());

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.tellStatus";
    ariaSend.params = listString;
    ;

    return GetResult<AriaTellStatus>(ariaSend);
}

AriaChangeOption AriaClient::ChangeOptionAsync(const std::string& gid, const ListString& option)
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);
    listString.emplace_back(gid);
    listString.insert(listString.end(), option.begin(), option.end());

    AriaSendData ariaSend;
    QUuid id = QUuid::createUuid();
    ariaSend.id = id.toString().toStdString();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params = listString;

    return GetResult<AriaChangeOption>(ariaSend);
}

std::string AriaClient::Request(const std::string& url, const std::string& param)
{
    std::string strResponse;
    HttpPost(url, param, strResponse);
    return strResponse;
}

}  // namespace aria2net
