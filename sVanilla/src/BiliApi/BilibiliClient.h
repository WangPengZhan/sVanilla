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
    std::string m_cookie = "buvid3=ADA66B09-D132-6BE1-CFDD-CBCE6127FDDC20905infoc; b_nut=1700031420; _uuid=41BF94EC-FDB3-254C-E88E-1BDC37B2234121570infoc; "
                           "buvid4=9FDFF631-9FAB-15A3-FB9B-A6EDF5D2365821905-023111506-; buvid_fp=aa63743432b55417f13486a297c9e387; CURRENT_FNVAL=4048; "
                           "rpdid=|(k)~YJmllk|0J'u~||~|k)|l; DedeUserID=1524815138; DedeUserID__ckMd5=f492745a735982a3; CURRENT_QUALITY=112; "
                           "bsource=search_google; enable_web_push=DISABLE; header_theme_version=CLOSE; home_feed_column=4; PVID=1; "
                           "browser_resolution=1212-889; bp_video_offset_1524815138=885950391975411751; "
                           "SESSDATA=6d4e84c2%2C1720687056%2C60501%2A11CjBDEcezvw8qxNpNlRJzZ21GzzkaWk6KR3ojBIezftZpxKv0_"
                           "p2kdeajMhZnfso068YSVkM1cW0xSjktQkR0RXg5UHF4alZQZmxYSjQ3aTJabW1nTUdFZFc5NzYwYXBHOGpXM0ZZTDFsSXpUMWJaRkJEeTlfRFZPWUFRUlBtemFXUWFUc3RX"
                           "MnhRIIEC; bili_jct=4d96cfb186e63843cd03c8e59d9481e7; sid=5us2r88l; b_lsid=6FB5BE3F_18D390DAA43";

private:
    std::string m_wbiKey;
};

}  // namespace BiliApi
