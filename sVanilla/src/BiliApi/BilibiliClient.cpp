#include <QApplication>
#include <QFile>

#include <fstream>

#include "BilibiliClient.h"
#include "Logger/Logger.h"
#include "Util/JsonProcess.h"

namespace BiliApi
{

BilibiliClient& BilibiliClient::globalClient()
{
    static BilibiliClient bilibiliClient;
    return bilibiliClient;
}

VideoView BilibiliClient::GetVideoView(const std::string& bvid)
{
    std::string url = videoViewUrl + std::string("?bvid=") + bvid;
    std::string response;
    HttpGet(url, response);
    return VideoView(GetDataFromRespones(response));
}

PlayUrl BilibiliClient::GetPlayUrl(long long cid, long long qn, const std::string& bvid)
{
    // "https://api.bilibili.com/x/player/playurl?cid=%s&qn=125&fourk=1&fnver=0&fnval=4048&bvid=%s";
    std::string url = videoPlayUrl + std::string("?from_client=BROWSER&fourk=1&fnver=0&fnval=4048&cid=") + std::to_string(cid) + "&qn=" + std::to_string(qn) +
                      "&from_client=BROWSER&fourk=1&fnver=0&fnval=4048&bvid=" + bvid;
    std::string response;
    InitBiliDefaultHeaders();
    HttpGet(url, response);
    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeaders();
    return PlayUrl(GetDataFromRespones(response));
}

LoginUrl BilibiliClient::GetLoginUrl()
{
    std::string url = "https://passport.bilibili.com/x/passport-login/web/qrcode/generate";
    std::string response;

    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeadersLogin();
    HttpGet(url, response);
    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeaders();

    qDebug() << QString::fromStdString(response);
    return LoginUrl(GetDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::GetLoginStatus(const std::string& qrcode_key)
{
    std::string url = "https://passport.bilibili.com/x/passport-login/web/qrcode/poll";
    std::string response;

    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeadersLogin();
    // ParamType param{
    //     {"qrcode_key", qrcode_key                  },
    //     {"gourl",    "https://www.bilibili.com"},
    // };
    url = url + "?qrcode_key=" + qrcode_key;
    // HttpPost(url, param, response);
    HttpGet(url, response);
    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeaders();

    qDebug() << "status response：" << QString::fromStdString(response);
    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(response);
    }
    catch (std::exception& e)
    {
        return {};
    }
    return LoginStatusScanning(json);
}

void BilibiliClient::InitBiliDefaultHeaders()
{
    m_headers = curl_slist_append(m_headers, "Referer: https://www.bilibili.com");
}

void BilibiliClient::SetLogined(bool logined)
{
    m_logined = logined;
}

bool BilibiliClient::GetLogined() const
{
    return m_logined;
}

nlohmann::json BilibiliClient::GetDataFromRespones(const std::string& respones)
{
    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(respones);
        util::JsonProcess::removeNullValues(json);
    }
    catch (std::exception& e)
    {
        return json;
    }
    std::ofstream stream("response.json", std::ios_base::out | std::ios_base::app);
    stream << json;
    stream.flush();
    stream.close();
    return json["data"];
    // return json;
}

BilibiliClient::BilibiliClient()
    : CNetWork()
    , m_logined(false)
{
    std::string origin = "origin: ";
    AppendHeaders(origin + mainUrl);
    AppendHeaders("Content-Type: application/json;");
}

}  // namespace BiliApi
