#pragma once

#include <QObject>

#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include <string>

#include "BiliApi.h"
#include "NetWork/CNetWork.h"

namespace BiliApi
{

constexpr char const mainUrl[] = "https://www.bilibili.com/";
constexpr char const videoViewUrl[] = "https://api.bilibili.com/x/web-interface/view";
constexpr char const videoPlayUrl[] = "https://api.bilibili.com/x/player/wbi/playurl";

constexpr char passBaseUrl[] = "https://passport.bilibili.com/";
constexpr char loginUrl[] = "x/passport-login/web/qrcode/generate";
constexpr char loginStatusUrl[] = "x/passport-login/web/qrcode/poll";

class BilibiliClient : public CNetWork
{
public:
    BilibiliClient();
    ~BilibiliClient() = default;

    static BilibiliClient& globalClient();

    VideoViewOrigin GetVideoView(const std::string& bvid);
    PlayUrlOrigin GetPlayUrl(long long cid, long long qn, const std::string& bvid);
    LoginUrlOrigin GetLoginUrl();
    LoginStatusScanning GetLoginStatus(const std::string& qrcode_key);
    std::string GetWbiKey();
    void ResetWbi();

    void InitBiliDefaultHeaders();

    void SetLogined(bool logined);
    bool GetLogined() const;
    void Rquest(HpptType method, const std::string& url, const ParamType& param, std::string& response, const std::list<std::string>& headers, bool needCookie);

    std::list<std::string> getPassportHeaders();
    std::list<std::string> getDefalutHeaders();
    void ParseCookie(const std::string& url);
    static nlohmann::json GetDataFromRespones(const std::string& respones);

    volatile bool m_logined;
    std::string m_cookie =
        "DedeUserID=3493257237498504;DedeUserID__ckMd5=59d6f8a3796205d8;Expires=1722504588;SESSDATA=740afe11,1722504588,e6dd0*22CjAcqHYH5qc5hlCr-BqRyymwASqS_"
        "8kZXBhWDwipyoNpwIA5Ch3kePQMZraXL0uL2GISVmg3blFmeDg3TjlRQklJbDhmZnJ2NG9vZW1Na3c5ZS00VXhmME1hMDYyRGV4NFhBaWZyMGo3NmRGMC1WMzBTMnJXaUNxcW52cGxBaUo3bDQ2Ri1"
        "kTFp3IIEC;bili_jct=8f558ffa014858af89394be0922a629e;gourl=https%3A%2F%2Fwww.bilibili.com";

private:
    std::string m_wbiKey;
};

}  // namespace BiliApi
