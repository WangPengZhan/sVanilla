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
#include <sstream>

namespace BiliApi
{

BilibiliClient& BilibiliClient::globalClient()
{
    static BilibiliClient bilibiliClient;
    return bilibiliClient;
}
LoginUrlOrigin BilibiliClient::GetLoginUrl()
{
    std::string response;

    Rquest(GET, PassportURL::QRCode, {}, response, getPassportHeaders(), false);

    qDebug() << QString::fromStdString(response);
    return LoginUrlOrigin(GetDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::GetLoginStatus(const std::string& qrcode_key)
{
    ParamType param;
    param["qrcode_key"] = qrcode_key;

    std::string response;
    Rquest(GET, PassportURL::LoginStatus, param, response, getPassportHeaders(), false);

    qDebug() << "status response：" << QString::fromStdString(response);

    return LoginStatusScanning(GetDataFromRespones(response));
}

VideoViewOrigin BilibiliClient::GetVideoView(const std::string& bvid)
{
    ParamType param;
    param["bvid"] = bvid;

    std::string response;
    Rquest(GET, VideoURL::View, param, response, getDefalutHeaders(), true);

    return VideoViewOrigin(GetDataFromRespones(response));
}

PlayUrlOrigin BilibiliClient::GetPlayUrl(long long cid, long long qn, const std::string& bvid)
{
    ParamType param;
    param["bvid"] = bvid;
    param["cid"] = std::to_string(cid);
    param["qn"] = std::to_string(qn);
    param["fnver"] = "0";  // 恒为零
    param["fnval"] = "0";
    param["fourk"] = "1";
    ResetWbi();
    encWbi(param, m_wbiKey);
    std::string response;
    Rquest(GET, VideoURL::Playurl, param, response, getDefalutHeaders(), true);

    qDebug() << "playurl response：" << QString::fromStdString(response);
    return PlayUrlOrigin(GetDataFromRespones(response));
}

void BilibiliClient::ResetWbi()
{
    std::string response;
    Rquest(GET, PassportURL::WebNav, {}, response, {}, true);
    qDebug() << "nav response：" << QString::fromStdString(response);
    std::string img_url;
    std::string sub_url;
    if (const auto res = NavData(GetDataFromRespones(response)); !res.img.empty())
    {
        img_url = std::filesystem::path(res.img[0]).stem().string();
        sub_url = std::filesystem::path(res.img[1]).stem().string();
    }
    if (!img_url.empty() && !sub_url.empty())
    {
        m_wbiKey = GetMixinKey(img_url + sub_url);
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
        if (const std::string cookie = readFromFile("cookie"); !cookie.empty())
        {
            HttpGet(url, param, response, cookie);
        }
    }
    else if (method == POST)
    {
        HttpPost(url, param, response);
    }
    else
    {
        HttpGet(url, param, response);
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

void BilibiliClient::ParseCookie(const std::string& url)
{
    std::string cookie = url.substr(url.find('?') + 1);
    replaceCharacter(cookie, "&", ";");
    nlohmann::json j_cookie;
    std::istringstream iss(url);
    std::string segment;
    while (std::getline(iss, segment, ';'))
    {
        std::istringstream iss2(segment);
        std::string value;
        if (std::string key; std::getline(std::getline(iss2, key, '='), value))
        {
            if (key == "Expires" || key == "SESSDATA" || key == "bili_jct")
            {
                j_cookie[key] = value;
            }
        }
    }
    m_cookie = "SESSDATA=" + std::string(j_cookie["SESSDATA"]);
    nlohmann::json bilibili_data;
    bilibili_data["cookie"] = j_cookie;
    saveToFile("cookie", cookie);
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
