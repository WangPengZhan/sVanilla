#pragma once

#include <QObject>

#include <nlohmann/json.hpp>

#include "NetWork/CNetWork.h"
#include "Aria2Net/Protocol/Protocol.h"

namespace aria2net
{
const std::string GetRpcUri(int listenPort = 6800);
const std::string GetGuid();

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

    AriaClient() = default;
    ~AriaClient() = default;

    AriaClient(const AriaClient& other) = delete;
    AriaClient& operator=(const AriaClient& other) = delete;

    static AriaClient& globalClient();

    SystemListNotifications listNotificationsAsync();
    SystemListMethods listMethodsAsync();
    std::list<SystemMulticall> multicallAsync(const std::list<SystemMulticallMathod>& systemMulticallMathods);
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
    AriaAddUri AddUriAsync(ListString uris, AriaSendOption option, int position = -1);
    AriaTellStatus TellStatus(const std::string& gid);

private:
    static constexpr char JSONRPC[] = "2.0";
    static constexpr char TOKEN[] = "sVanila";


    template <typename Result> Result GetResult(const AriaSendData& sendData);

public:
    std::string Request(const std::string& url, const std::string& params);
};

template <typename Result> inline Result AriaClient::GetResult(const AriaSendData& sendData)
{
    std::string strParams = sendData.toString();
    std::string strResponse = Request(GetRpcUri(), strParams);
    nlohmann::json result = nlohmann::json::parse(strResponse);
    return Result(result);
}

}  // namespace aria2net
