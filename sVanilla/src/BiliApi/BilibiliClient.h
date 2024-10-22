#pragma once
#include <string>

#include "BiliApi.h"
#include "NetWork/CNetWork.h"
#include "NetWork/CurlCpp/CurlCookieJar.h"

namespace biliapi
{

enum class BusinessType
{
    Archive,
    Pgc,
    Live,
    Article_list,
    Article,

    Unkown = 255,
};

std::string to_string(BusinessType businessType);

enum HistoryType
{
    All,
    Archive,
    Live,
    Article,
    Unkown = 255,
};

std::string to_string(HistoryType historyType);

struct HistoryQueryParam
{
    int max{};
    BusinessType business{BusinessType::Unkown};
    uint64_t view_at{};
    HistoryType type{HistoryType::Unkown};
    int ps{};

    network::NetWork::ParamType toParam() const;
};

class BilibiliClient : public network::NetWork
{
public:
    BilibiliClient();
    ~BilibiliClient() = default;

    static BilibiliClient& globalClient();

    VideoViewOrigin getVideoView(const std::string& bvid);
    PlayUrlOrigin getPlayUrl(long long cid, long long qn, const std::string& bvid, long long fnval);
    LoginUrlOrigin getLoginUrl();
    LoginStatusScanning getLoginStatus(const std::string& qrcodeKey);
    Nav getNavInfo();
    LogoutExitV2 getLogoutExitV2();
    History getHistory(HistoryQueryParam param);

    [[nodiscard]] bool isLogined() const;

    void resetWbi();
    void encodeWithWbi(ParamType& params);

    static void parseCookie(const std::string& url);
    static nlohmann::json getDataFromRespones(const std::string& respones);

    static network::CurlHeader passPortHeaders();

protected:
    void initDefaultHeaders();
    void initDefaultOptions();

private:
    network::CurlCookies m_cookies;  // we sellect this mode for debug
};

}  // namespace biliapi
