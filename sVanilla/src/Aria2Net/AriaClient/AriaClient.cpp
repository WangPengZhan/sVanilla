#include <QUuid>

#include "AriaClient.h"
#include "Logger/Logger.h"

namespace aria2net
{

std::string GetRpcUri(int listenPort)
{
    return QString("http://localhost:%1/jsonrpc").arg(listenPort).toStdString();
}

std::string GetGuid()
{
    QUuid id = QUuid::createUuid();
    std::string strGuid = id.toString(QUuid::Id128).toStdString();
    return strGuid;
}

AriaClient& AriaClient::globalClient()
{
    static AriaClient ariaClient;
    return ariaClient;
}

SystemListNotifications AriaClient::listNotificationsAsync()
{
    AriaSendData ariaSend;

    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.listNotifications";

    return GetResult<SystemListNotifications>(ariaSend);
}

SystemListMethods AriaClient::listMethodsAsync()
{
    AriaSendData ariaSend;

    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.listMethods";

    return GetResult<SystemListMethods>(ariaSend);
}

std::list<SystemMulticall> AriaClient::multicallAsync(const std::list<SystemMulticallMathod>& systemMulticallMathods)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "system.multicall";
    ariaSend.params[0] = systemMulticallMathods;

    auto jsonResult = GetResult<nlohmann::json>(ariaSend);
    std::list<SystemMulticall> result;
    for (const auto& sigleJson : jsonResult)
    {
        result.emplace_back(sigleJson);
    }

    return result;
}

AriaSaveSession AriaClient::saveSessionAsync()
{
    ListString listString;
    listString.emplace_back(std::string("token:") + TOKEN);

    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.saveSession";
    ariaSend.params[0] = listString;
    return GetResult<AriaSaveSession>(ariaSend);
}

AriaShutdown AriaClient::ForceShutdownAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.forceShutdown";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaShutdown>(ariaSend);
}

AriaShutdown AriaClient::ShutdownAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.shutdown";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaShutdown>(ariaSend);
}

AriaGetSessionInfo AriaClient::GetSessionInfoAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getSessionInfo";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaGetSessionInfo>(ariaSend);
}

AriaVersion AriaClient::GetAriaVersionAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getVersion";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaVersion>(ariaSend);
}

AriaRemove AriaClient::RemoveDownloadResultAsync(const std::string& gid)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.removeDownloadResult";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;

    return GetResult<AriaRemove>(ariaSend);
}

AriaRemove AriaClient::PurgeDownloadResultAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.purgeDownloadResult";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaRemove>(ariaSend);
}

AriaGetGlobalStat AriaClient::GetGlobalStatAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaGetGlobalStat>(ariaSend);
}

AriaChangeOption AriaClient::ChangeGlobalOptionAsync(const ListString& option)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = option;

    return GetResult<AriaChangeOption>(ariaSend);
}

AriaGetOption AriaClient::GetGlobalOptionAsync()
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalOption";
    ariaSend.params[0] = std::string("token:") + TOKEN;

    return GetResult<AriaGetOption>(ariaSend);
}

AriaGetOption AriaClient::GetOptionAsync(const std::string& gid)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getOption";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;

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
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.changePosition";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;
    ariaSend.params[2] = std::to_string(fileIndex);
    ariaSend.params[3] = delUris;
    ariaSend.params[4] = addUris;

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
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.changePosition";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;
    ariaSend.params[2] = std::to_string(pos);
    ariaSend.params[3] = std::to_string(how);

    return GetResult<AriaChangePosition>(ariaSend);
}

AriaAddUri AriaClient::AddUriAsync(const ListString& uris, AriaSendOption option, int position)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.addUri";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = uris;
    ariaSend.params[2] = option;

    return GetResult<AriaAddUri>(ariaSend);
}

AriaTellStatus AriaClient::TellStatus(const std::string& gid)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.tellStatus";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;

    return GetResult<AriaTellStatus>(ariaSend);
}

AriaChangeOption AriaClient::ChangeOptionAsync(const std::string& gid, const ListString& option)
{
    AriaSendData ariaSend;
    ariaSend.id = GetGuid();
    ariaSend.jsonrpc = JSONRPC;
    ariaSend.method = "aria2.getGlobalStat";
    ariaSend.params[0] = std::string("token:") + TOKEN;
    ariaSend.params[1] = gid;
    ariaSend.params[2] = option;

    return GetResult<AriaChangeOption>(ariaSend);
}

std::string AriaClient::Request(const std::string& url, const std::string& param)
{
    std::string strResponse;
    HttpPost(url, param, strResponse);
    return strResponse;
}

std::string AriaClient::ConstructURL() const
{
    std::string const url = m_settings->readString("App", "RPCAddress");
    std::string const port = m_settings->readString("App", "RPCPort");
    return url + ":" + port + "/jsonrpc";
}
std::string AriaClient::GetToken() const
{
    return m_settings->readString("App", "TOKEN");
}

std::string AriaClient::ConstructSendData(const std::string& methodName, array params)
{
    AriaSendData data;
    data.id = QUuid::createUuid().toString(QUuid::Id128).toStdString();
    data.jsonrpc = "2.0";
    data.method = "aria2." + std::string(methodName);
    data.params.reserve(params.size() + 1);
    data.params = std::move(params);
    data.params.insert(data.params.begin(), "token:" + GetToken());
    return Request(ConstructURL(), data.toString());
}

}  // namespace aria2net
