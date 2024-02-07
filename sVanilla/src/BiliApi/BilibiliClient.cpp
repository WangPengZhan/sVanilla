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
    param.emplace("bvid", bvid);
    param.emplace("cid", std::to_string(cid));
    param.emplace("qn", std::to_string(qn));
    param.emplace("fnver", "0");
    param.emplace("fnval", "0");
    param.emplace("fourk", "1");

    encWbi(param);

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
        nlohmann::json j;
        j.emplace("mixin_key", GetMixinKey(img_url + sub_url));
        j.emplace("Expires", std::time(nullptr) + 60 * 60 * 24);
        updateData("sVanilla.data", "mixinKey", j);
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
        if (Cookie cookie; readData<Cookie>("sVanilla.data", "cookie", cookie))
        {
            HttpGet(url, param, response, cookie.SESSDATA);
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
void BilibiliClient::encWbi(ParamType& params)
{
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
        sortedParams.push_back(url_encode(key) + "=" += url_encode(filteredValue));
    }
    std::sort(sortedParams.begin(), sortedParams.end());
    // 序列化参数
    const std::string query = std::accumulate(std::next(sortedParams.begin()), sortedParams.end(), sortedParams[0], [](std::string a, std::string b) {
        return std::move(a) + '&' + std::move(b);
    });

    // 计算 w_rid 字段
    if (MixinKey mixinKey; readData<MixinKey>("sVanilla.data", "mixinKey", mixinKey))
    {
        params["w_rid"] = MD5Hash(query + mixinKey.value);
    }
    else
    {
        ResetWbi();
        encWbi(params);
    }
}

void BilibiliClient::ParseCookie(const std::string& url)
{
    std::string cookie = url.substr(url.find('?') + 1);
    replaceCharacter(cookie, "&", ";");
    nlohmann::json j;
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
                j.emplace(key, value);
            }
        }
    }
    m_cookie = "SESSDATA=" + std::string(j["SESSDATA"]);
    updateData("sVanilla.data", "cookie", j);
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
