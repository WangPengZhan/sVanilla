#pragma once

#include <QDebug>

#include <nlohmann/json.hpp>

#include <utility>

#include "Util/Setting.h"
#include "NetWork/CNetWork.h"
#include "Aria2Net/Protocol/Protocol.h"

namespace aria2net
{
std::string GetRpcUri(int listenPort = 6800);
std::string GetGuid();

class AriaClient : public network::NetWork
{
public:
    enum HowChangePosition
    {
        POS_SET = 1,
        POS_CUR,
        POS_END
    };

    using ListString = std::list<std::string>;
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
    AriaRemove RemoveAsync(const std::string& gid);
    AriaPause PauseAsync(const std::string& gid);
    AriaPause UnpauseAsync(const std::string& gid);

private:
    static constexpr char JSONRPC[] = "2.0";
    static constexpr char TOKEN[] = "sVanilla";
    [[nodiscard]] std::string ConstructURL() const;
    [[nodiscard]] std::string GetToken() const;
    std::string ConstructSendData(std::string methodName, nlohmann::json::array_t params);
    std::string Request(const std::string& url, const std::string& params);
    template <typename Result>
    Result GetResult(const AriaSendData& sendData);

public:
    std::shared_ptr<Settings> m_settings;
    template <typename Result>
    Result Call(std::string methodName, nlohmann::json::array_t params);
};

template <typename Result>
Result AriaClient::Call(std::string methodName, nlohmann::json::array_t params)
{
    std::string res = ConstructSendData(std::move(methodName), std::move(params));
    if (res.empty())
    {
        return Result();
    }
    nlohmann::json result = nlohmann::json::parse(res);
    return Result(result);
}

template <typename Result>
inline Result AriaClient::GetResult(const AriaSendData& sendData)
{
    std::string strParams = sendData.toString();
    std::string strResponse = Request(GetRpcUri(), strParams);
    nlohmann::json result = nlohmann::json::parse(strResponse);
    return Result(result);
}

}  // namespace aria2net
