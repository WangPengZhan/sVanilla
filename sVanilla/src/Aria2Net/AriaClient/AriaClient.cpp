#include <QUuid>
#include <QDebug>

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
    return Call<SystemListNotifications>("system.listNotifications", {});
}

SystemListMethods AriaClient::listMethodsAsync()
{
    return Call<SystemListNotifications>("system.listMethods", {});
}

std::list<SystemMulticall> AriaClient::multicallAsync(const std::list<SystemMulticallMathod>& systemMulticallMathods)
{
    auto jsonResult = Call<nlohmann::json>("system.listMethods", {systemMulticallMathods});
    std::list<SystemMulticall> result;
    for (const auto& sigleJson : jsonResult)
    {
        result.emplace_back(sigleJson);
    }
    return result;
}

AriaSaveSession AriaClient::saveSessionAsync()
{
    return Call<AriaSaveSession>("aria2.saveSession", {});
}

AriaShutdown AriaClient::ForceShutdownAsync()
{
    return Call<AriaShutdown>("aria2.forceShutdown", {});
}

AriaShutdown AriaClient::ShutdownAsync()
{
    return Call<AriaShutdown>("aria2.shutdown", {});
}

AriaGetSessionInfo AriaClient::GetSessionInfoAsync()
{
    return Call<AriaGetSessionInfo>("aria2.getSessionInfo", {});
}

AriaVersion AriaClient::GetAriaVersionAsync()
{
    return Call<AriaVersion>("aria2.getVersion", {});
}

AriaRemove AriaClient::RemoveDownloadResultAsync(const std::string& gid)
{
    return Call<AriaRemove>("aria2.removeDownloadResult", {gid});
}

AriaRemove AriaClient::PurgeDownloadResultAsync()
{
    return Call<AriaRemove>("aria2.purgeDownloadResult", {});
}

AriaGetGlobalStat AriaClient::GetGlobalStatAsync()
{
    return Call<AriaGetGlobalStat>("aria2.getGlobalStat", {});
}

AriaChangeOption AriaClient::ChangeGlobalOptionAsync(const ListString& option)
{
    return Call<AriaChangeOption>("aria2.getGlobalStat", {option});
}

AriaGetOption AriaClient::GetGlobalOptionAsync()
{
    return Call<AriaGetOption>("aria2.getGlobalStat", {});
}

AriaGetOption AriaClient::GetOptionAsync(const std::string& gid)
{
    return Call<AriaGetOption>("aria2.getOption", {gid});
}

AriaChangeUri AriaClient::ChangeUriAsync(const std::string& gid, int fileIndex, ListString delUris, ListString addUris, int position)
{
    return Call<AriaChangeUri>("aria2.changePosition", {gid, fileIndex, delUris, addUris, position});
}

AriaChangePosition AriaClient::ChangePositionAsync(const std::string& gid, int pos, HowChangePosition how)
{
    return Call<AriaChangePosition>("aria2.changePosition", {gid, pos, how});
}

AriaAddUri AriaClient::AddUriAsync(const ListString& uris, AriaSendOption option, int position)
{
    if (position < 0)
    {
        return Call<AriaAddUri>("aria2.addUri", {uris, option});
    }
    else
    {
        return Call<AriaAddUri>("aria2.addUri", {uris, option, position});
    }
}

AriaTellStatus AriaClient::TellStatus(const std::string& gid)
{
    return Call<AriaTellStatus>("aria2.tellStatus", {gid});
}

AriaChangeOption AriaClient::ChangeOptionAsync(const std::string& gid, const ListString& option)
{
    return Call<AriaChangeOption>("aria2.getGlobalStat", {gid});
}

std::string AriaClient::Request(const std::string& url, const std::string& param)
{
    std::string strResponse;
    HttpPost(url, param, strResponse);
    return strResponse;
}

std::string AriaClient::ConstructURL() const
{
    return GetRpcUri(6800);
    std::string const url = m_settings->readString("App", "RPCAddress");
    std::string const port = m_settings->readString("App", "RPCPort");
    return url + ":" + port + "/jsonrpc";
}
std::string AriaClient::GetToken() const
{
    return TOKEN;
    return m_settings->readString("App", "TOKEN");
}

std::string AriaClient::ConstructSendData(std::string methodName, nlohmann::json::array_t params)
{
    AriaSendData data;
    data.id = QUuid::createUuid().toString(QUuid::Id128).toStdString();
    data.jsonrpc = "2.0";
    data.method = std::move(methodName);
    data.params.reserve(params.size() + 1);
    data.params = std::move(params);
    data.params.insert(data.params.begin(), "token:" + GetToken());
    if (data.params.size() > 2 && data.params[2].empty())
    {
        data.params[2] = nlohmann::json::object();
    }
    qDebug() << "Aria2Client Rquest Body: " << data.toString().c_str();
    return Request(ConstructURL(), data.toString());
}

}  // namespace aria2net
