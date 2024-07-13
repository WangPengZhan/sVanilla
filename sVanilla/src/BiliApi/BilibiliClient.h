#pragma once
#include <string>

#include "BiliApi.h"
#include "NetWork/CNetWork.h"

namespace biliapi
{

class BilibiliClient : public network::NetWork
{
public:
    BilibiliClient();
    ~BilibiliClient() = default;

    static BilibiliClient& globalClient();

    VideoViewOrigin getVideoView(const std::string& bvid);
    PlayUrlOrigin getPlayUrl(long long cid, long long qn, const std::string& bvid);
    LoginUrlOrigin getLoginUrl();
    LoginStatusScanning getLoginStatus(const std::string& qrcodeKey);

    void setLogined(bool logined);
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
    bool m_logined;
};

}  // namespace biliapi
