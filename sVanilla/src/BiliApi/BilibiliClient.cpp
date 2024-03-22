#include <QApplication>

#include <curl/curl.h>
#include <regex>
#include <filesystem>
#include <sstream>
#include <string>
#include <fstream>

#include "BilibiliClient.h"
#include "Logger/Logger.h"
#include "Util/JsonProcess.h"
#include "BiliApiConstants.h"
#include "BilibiliUtils.h"
#include "NetWork/NetworkLog.h"

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
    PRINTJ("login response: ", response);
    return LoginUrlOrigin(GetDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::GetLoginStatus(const std::string& qrcode_key)
{
    ParamType param;
    param["qrcode_key"] = qrcode_key;

    std::string response;
    Rquest(GET, PassportURL::LoginStatus, param, response, getPassportHeaders(), false);
    PRINTJ("login status response: ", response);
    return LoginStatusScanning(GetDataFromRespones(response));
}

VideoViewOrigin BilibiliClient::GetVideoView(const std::string& bvid)
{
    ParamType param;
    param["bvid"] = bvid;

    std::string response;
    Rquest(GET, VideoURL::View, param, response, getDefalutHeaders(), false);
    PRINTJ("view response: ", response);
    return VideoViewOrigin(GetDataFromRespones(response));
}

PlayUrlOrigin BilibiliClient::GetPlayUrl(long long cid, long long qn, const std::string& bvid)
{
    ParamType param;
    param.emplace("bvid", bvid);
    param.emplace("cid", std::to_string(cid));
    param.emplace("qn", std::to_string(qn));
    param.emplace("fnver", "0");
    param.emplace("fnval", "0");
    param.emplace("fourk", "1");

    EncodeWithWbi(param);

    std::string response;
    Rquest(GET, VideoURL::Playurl, param, response, getDefalutHeaders(), true);

    PRINTJ("playurl response: ", response);
    return PlayUrlOrigin(GetDataFromRespones(response));
}

void BilibiliClient::Rquest(const HpptType method, const std::string& url, const ParamType& param, std::string& response, const std::list<std::string>& headers,
                            const bool needCookie)
{
    if (method == GET && needCookie)
    {
        if (Cookie cookie; readData(cookie, "cookie"))
        {
            const std::string cookieStr = "Cookie: SESSDATA=" + urlEncode(cookie.SESSDATA) + ";";
            PRINTS("Cookie", cookieStr);
            std::list<std::string> headersWithCookie = headers;
            headersWithCookie.push_back(cookieStr);
            HttpGet(url, param, response, headersWithCookie);
        }
        else
        {
            // 重新获取cookie
        }
    }
    else if (method == POST)
    {
        HttpPost(url, param, response);
    }
    else
    {
        HttpGet(url, param, response, headers);
    }
}

void BilibiliClient::ResetWbi()
{
    std::string response;
    Rquest(GET, PassportURL::WebNav, {}, response, getDefalutHeaders(), false);
    PRINTJ("nav response: ", response);
    std::string img_url;
    std::string sub_url;
    try
    {
        auto jsonData = nlohmann::json::parse(response);
        img_url = jsonData["data"]["wbi_img"]["img_url"].get<std::string>();
        sub_url = jsonData["data"]["wbi_img"]["sub_url"].get<std::string>();
    }
    catch (nlohmann::json::parse_error& e)
    {
        PRINTS("Error parsing NAV response: ", e.what());
    }
    if (!img_url.empty() && !sub_url.empty())
    {
        img_url = std::filesystem::path(img_url).stem().string();
        sub_url = std::filesystem::path(sub_url).stem().string();
        nlohmann::json j;
        j.emplace("mixin_key", GetMixinKey(img_url + sub_url));
        j.emplace("Expires", (std::time(nullptr) + 60 * 60 * 24) / 86400);  // 有效期一天
        updateData("mixinKey", j);
    }
}

void BilibiliClient::EncodeWithWbi(ParamType& params)
{
    MixinKey mixinKey;
    if (!readData(mixinKey, "mixinKey"))
    {
        ResetWbi();
        readData(mixinKey, "mixinKey");
    }
    // 添加 wts 字段
    const time_t curr_time = time(nullptr);
    params["wts"] = std::to_string(curr_time);

    // 按照 key 重排参数
    std::vector<std::string> sortedParams;
    for (auto& [key, value] : params)
    {
        // 过滤 value 中的 "!'()*" 字符
        const std::string filteredValue = filterCharacters(value);
        // url encode
        sortedParams.push_back(urlEncode(key) + "=" += urlEncode(filteredValue));
    }
    std::sort(sortedParams.begin(), sortedParams.end());
    // 序列化参数
    const std::string query = std::accumulate(std::next(sortedParams.begin()), sortedParams.end(), sortedParams[0], [](std::string a, std::string b) {
        return std::move(a) + '&' + std::move(b);
    });

    // 计算 w_rid 字段
    params["w_rid"] = MD5Hash(query + mixinKey.mixin_key);
}

void BilibiliClient::ParseCookie(const std::string& url)
{
    nlohmann::json result;
    std::string params = url.substr(url.find('?') + 1);  // 获取参数部分
    std::stringstream ss(params);
    std::string param;
    while (std::getline(ss, param, '&'))
    {
        std::string key = param.substr(0, param.find('='));
        std::string value = param.substr(param.find('=') + 1);
        if (key == "Expires")
        {
            // 将 Expires 时间戳字符串转换为天数, 以便与 mixinKey 日更同步
            auto expires = static_cast<std::time_t>(std::stoll(value));
            result.emplace("Expires", expires / 86400);
        }
        else if (key == "SESSDATA" || key == "bili_jct")
        {
            result.emplace(key, value);
        }
    }
    updateData("cookie", result);
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

void BilibiliClient::SetLogined(bool logined)
{
    m_logined = logined;
}

bool BilibiliClient::GetLogined() const
{
    return m_logined;
}

void BilibiliClient::InitBiliDefaultHeaders()
{
    m_headers = curl_slist_append(m_headers, "Referer: https://www.bilibili.com");
}

std::list<std::string> BilibiliClient::getPassportHeaders()
{
    return {
        getAgent(),
        Headers::PassportReferer,
    };
}
std::list<std::string> BilibiliClient::getDefalutHeaders()
{
    return {
        getAgent(),
        Headers::DefaultReferer,
    };
}


BilibiliClient::BilibiliClient()
    : m_logined(false)
{
    std::string origin = "origin: ";
    AppendHeaders(origin + mainUrl);
    AppendHeaders("Content-Type: application/json;");
}

}  // namespace BiliApi
