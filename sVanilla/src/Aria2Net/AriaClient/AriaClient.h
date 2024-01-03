#pragma once

#include <QObject>

#include <nlohmann/json.hpp>
#include <Util/Setting.h>
#include "NetWork/CNetWork.h"
#include "Aria2Net/Protocol/Protocol.h"

namespace aria2net
{
std::string GetRpcUri(int listenPort = 6800);
std::string GetGuid();

class AriaClient : public CNetWork
{
public:
    enum HowChangePosition
    {
        POS_SET = 1,
        POS_CUR,
        POS_END
    };

    using ListString = std::list<std::string>;
    typedef nlohmann::json json;
    typedef nlohmann::json::array_t array;
    AriaClient() = default;
    ~AriaClient() = default;

    AriaClient(const AriaClient& other) = delete;
    AriaClient& operator=(const AriaClient& other) = delete;

    static AriaClient& globalClient();

    SystemListNotifications listNotificationsAsync();
    SystemListMethods listMethodsAsync();
    std::list<SystemMulticall> multicallAsync(const std::list<SystemMulticallMathod>& systemMulticallMethods);
    AriaSaveSession saveSessionAsync();
    AriaShutdown ForceShutdownAsync();
    AriaShutdown ShutdownAsync();
    AriaGetSessionInfo GetSessionInfoAsync();
    AriaVersion GetAriaVersionAsync();
    AriaRemove RemoveDownloadResultAsync(const std::string& gid);
    AriaRemove PurgeDownloadResultAsync();
    AriaGetGlobalStat GetGlobalStatAsync();
    AriaChangeOption ChangeGlobalOptionAsync(const ListString& option);
    AriaChangeOption ChangeOptionAsync(const std::string& gid, const ListString& option);
    AriaGetOption GetGlobalOptionAsync();
    AriaGetOption GetOptionAsync(const std::string& gid);
    AriaChangeUri ChangeUriAsync(const std::string& gid, int fileIndex, ListString delUris, ListString addUris, int position = -1);
    AriaChangePosition ChangePositionAsync(const std::string& gid, int pos, HowChangePosition how);
    // AriaTellStatusList TellStoppedAsync(int offset, int num);
    AriaAddUri AddUriAsync(const ListString& uris, AriaSendOption option, int position = -1);
    AriaTellStatus TellStatus(const std::string& gid);

private:
    static constexpr char JSONRPC[] = "2.0";
    static constexpr char TOKEN[] = "sVanilla";
    [[nodiscard]] std::string ConstructURL() const;
    [[nodiscard]] std::string GetToken() const;
    std::string ConstructSendData(const std::string& methodName, array params);
    std::string Request(const std::string& url, const std::string& params);
    template <typename Result> Result GetResult(const AriaSendData& sendData);

public:
    std::shared_ptr<Settings> m_settings;
    template <class Result> Result Call(const std::string& methodName, const array& params);
};
template <class Result> Result AriaClient::Call(const std::string& methodName, const array& params)
{
    std::string res = ConstructSendData(methodName, params);
    json result = json::parse(res);
    return Result(result);
}
template <typename Result> inline Result AriaClient::GetResult(const AriaSendData& sendData)
{
    std::string strParams = sendData.toString();
    std::string strResponse = Request(GetRpcUri(), strParams);
    nlohmann::json result = nlohmann::json::parse(strResponse);
    return Result(result);
}

}  // namespace aria2net
