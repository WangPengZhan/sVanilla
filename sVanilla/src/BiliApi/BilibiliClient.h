#pragma once
#include <nlohmann/json.hpp>
#include <string>
#include "BiliApi.h"
#include "NetWork/CNetWork.h"

namespace biliapi
{

constexpr char const mainUrl[] = "https://www.bilibili.com/";
constexpr char const videoViewUrl[] = "https://api.bilibili.com/x/web-interface/view";
constexpr char const videoPlayUrl[] = "https://api.bilibili.com/x/player/wbi/playurl";

constexpr char passBaseUrl[] = "https://passport.bilibili.com/";
constexpr char loginUrl[] = "x/passport-login/web/qrcode/generate";
constexpr char loginStatusUrl[] = "x/passport-login/web/qrcode/poll";

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
    bool isLogined() const;

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
