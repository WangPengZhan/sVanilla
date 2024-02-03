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

LoginUrlOrigin BilibiliClient::GetLoginUrl()
{
    std::string response;

    const auto headers = getPassportHeaders();
    Rquest(GET, PassportURL::QRCode, {}, response, headers, false);

    qDebug() << QString::fromStdString(response);
    return LoginUrlOrigin(GetDataFromRespones(response));
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
    const auto headers = getDefalutHeaders();
    std::string response;
    Rquest(GET, VideoURL::Playurl, param, response, headers, true);

    qDebug() << "playurl response：" << QString::fromStdString(response);
    return PlayUrlOrigin(GetDataFromRespones(response));
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
        HttpGet(url, param, response, m_cookie);
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
// 辅助函数，用于替换字符串中的所有目标子串为指定的新子串
void replaceAll(std::string& source, const std::string& from, const std::string& to) {
    std::string newString;
    newString.reserve(source.length());  // 预分配足够空间

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while (std::string::npos != (findPos = source.find(from, lastPos))) {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // 拼接最后一个分隔符后的所有字符
    newString += source.substr(lastPos);
    source.swap(newString);
}

void BilibiliClient::ParseCookie(const std::string& url)
{

    // std::string result;
    // std::string keys[] = {"DedeUserID=", "DedeUserID__ckMd5=", "SESSDATA=", "bili_jct="};
    // for (const auto& key : keys)
    // {
    //     if (size_t startPos = url.find(key); startPos != std::string::npos)
    //     {
    //         startPos += key.length();
    //         if (const size_t endPos = url.find('&', startPos); endPos != std::string::npos)
    //         {
    //             if (!result.empty())
    //             {
    //                 result += ";";  // 在不是第一个关键词值的前面加上分号
    //             }
    //             result += url.substr(startPos, endPos - startPos);
    //         }
    //     }
    // }
    std::string cookie = url.substr(url.find('?') + 1);
    replaceAll(cookie, "&", ";");
    m_cookie = cookie;
    // const std::regex reg("(^|&)?(\\w+)=([^&]+)(&|$)?");
    // if (std::smatch match; std::regex_search(url, match, reg))
    // {
    //     for (const auto& m : match)
    //     {
    //         if (m.str() == "SESSDATA")
    //         {
    //             m_SESSDATA = match.suffix().str();
    //         }
    //     }
    // }
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
