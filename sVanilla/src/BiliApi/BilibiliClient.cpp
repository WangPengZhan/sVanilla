#include <QApplication>
#include <QFile>

#include <fstream>

#include "BilibiliClient.h"
#include "Logger/Logger.h"
#include "Util/JsonProcess.h"
#include "BiliApiConstants.h"
#include "BilibiliUtils.h"

#include <regex>
#include <filesystem>

namespace BiliApi
{

BilibiliClient& BilibiliClient::globalClient()
{
    static BilibiliClient bilibiliClient;
    return bilibiliClient;
}

VideoViewOrigin BilibiliClient::GetVideoView(const std::string& bvid)
{
    ParamType param;
    param["bvid"] = bvid;

    const auto headers = getDefalutHeaders();

    std::string response;
    Rquest(GET, VideoURL::View, param, response, headers, true);

    return VideoViewOrigin(GetDataFromRespones(response));
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

LoginUrlOrigin BilibiliClient::GetLoginUrl()
{
    std::string response;

    const auto headers = getPassportHeaders();
    Rquest(GET, PassportURL::QRCode, {}, response, headers, false);

    qDebug() << QString::fromStdString(response);
    return LoginUrlOrigin(GetDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::GetLoginStatus(const std::string& qrcode_key)
{
    ParamType param;
    param["qrcode_key"] = qrcode_key;

    const auto headers = getPassportHeaders();

    std::string response;
    Rquest(GET, PassportURL::LoginStatus, param, response, headers, false);

    qDebug() << "status response：" << QString::fromStdString(response);

    return LoginStatusScanning(GetDataFromRespones(response));
}
std::string BilibiliClient::GetWbiKey()
{
    std::string response;
    Rquest(GET, PassportURL::WebNav, {}, response, {}, false);
    if (const auto res = NavData(GetDataFromRespones(response)); !res.img.empty())
    {
        return std::filesystem::path(res.img[0]).stem().string() + std::filesystem::path(res.img[1]).stem().string();
    }
    return "";
}
void BilibiliClient::ResetWbi()
{
    if (const auto key = GetWbiKey(); !key.empty())
    {
        m_wbiKey = GetMixinKey(key);
    }
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
void BilibiliClient::Rquest(const HpptType method, const std::string& url, const ParamType& param, std::string& response, const std::list<std::string>& headers,
                            const bool needCookie)
{
    curl_slist_free_all(m_headers);
    m_headers = nullptr;

    for (const auto& header : headers)
    {
        AppendHeaders(header);
    }
    if (method == GET && needCookie)
    {
        ParamType p;
        p.insert(param.begin(), param.end());
        p["SESSDATA"] = m_SESSDATA;
        HttpGet(url, p, response);
    }
    else if (method == POST)
    {
        HttpPost(url, param, response);
    }
    else
    {
        HttpGet(url, response);
    }
    curl_slist_free_all(m_headers);
    m_headers = nullptr;
    InitDefaultHeaders();
}
std::list<std::string> BilibiliClient::getPassportHeaders()
{
    return std::list{getAgent(), std::string("referer: https://passport.bilibili.com")};
}
std::list<std::string> BilibiliClient::getDefalutHeaders()
{
    return std::list{getAgent(), std::string("referer: https://www.bilibili.com")};
}
void BilibiliClient::SESSDATA(const std::string& url)
{
    const std::regex reg("(^|&)?(\\w+)=([^&]+)(&|$)?");
    if (std::smatch match; std::regex_search(url, match, reg))
    {
        for (const auto& m : match)
        {
            if (m.str() == "SESSDATA")
            {
                m_SESSDATA = match.suffix().str();
            }
        }
    }
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
        return nlohmann::json::object();
    }
    std::ofstream stream("response.json", std::ios_base::out | std::ios_base::app);
    stream << json;
    stream.flush();
    stream.close();
    return json;
    // return json;
}

BilibiliClient::BilibiliClient()
    : m_logined(false)
{
    std::string origin = "origin: ";
    AppendHeaders(origin + mainUrl);
    AppendHeaders("Content-Type: application/json;");
}

}  // namespace BiliApi
