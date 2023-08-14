#pragma once

#include <QObject>

#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include <string>

#include "NetWork/CNetWork.h"
#include "BiliApi.h"

namespace BiliApi
{

constexpr char const mainUrl[] = "https://www.bilibili.com/";
constexpr char const videoViewUrl[] = "http://api.bilibili.com/x/web-interface/view";
constexpr char const videoPlayUrl[] = "https://api.bilibili.com/x/player/playurl";

class BilibiliClient : public CNetWork
{
public:
    BilibiliClient();
    ~BilibiliClient() = default;

    static BilibiliClient& globalClient();

    VideoView GetVideoView(const std::string& bvid);
    PlayUrl GetPlayUrl(long long cid, long long qn, const std::string& bvid);
    LoginUrlOrigin GetLoginUrl();
    LoginStatusScanning GetLoginStatus(const std::string& oauthKey);
    
    void SetLogined(bool logined);
    bool GetLogined() const;

    
    static nlohmann::json GetDataFromRespones(const std::string& respones);

    volatile bool m_logined;
};

} // BiliApi

