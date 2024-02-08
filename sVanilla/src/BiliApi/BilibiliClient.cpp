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
#include <string>

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
    Rquest(GET, PassportURL::WebNav, {}, response, getDefalutHeaders(), false);
    qDebug() << "nav response：" << QString::fromStdString(response);
    std::string img_url;
    std::string sub_url;

    if (const auto res = Nav(GetDataFromRespones(response)); !res.data.wbi_img.img_url.empty())
    {
        img_url = std::filesystem::path(res.data.wbi_img.img_url).stem().string();
        sub_url = std::filesystem::path(res.data.wbi_img.sub_url).stem().string();
    }
    if (!img_url.empty() && !sub_url.empty())
    {
        nlohmann::json j;
        j.emplace("mixin_key", GetMixinKey(img_url + sub_url));
        j.emplace("Expires", (std::time(nullptr) + 60 * 60 * 24) / 86400);  // 有效期一天
        // j["mixin_key"] = GetMixinKey(img_url + sub_url);
        // j["Expires"] = (std::time(nullptr) + 60 * 60 * 24) / 86400;
        qDebug() << "mixin_key: " << QString::fromStdString(j.dump());
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
        if (const auto cookie = readCookie(); !cookie.SESSDATA.empty())
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
    MixinKey mixinKey = readMixinKey();
    if (mixinKey.mixin_key.empty())
    {
        ResetWbi();
        mixinKey = readMixinKey();
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
        sortedParams.push_back(url_encode(key) + "=" += url_encode(filteredValue));
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
    qDebug() << "cookie: " << QString::fromStdString(url);
    nlohmann::json result;
    std::string params = url.substr(url.find('?') + 1);  // 获取参数部分
    std::stringstream ss(params);
    std::string param;
    while (std::getline(ss, param, '&')) {
        std::string key = param.substr(0, param.find('='));
        std::string value = param.substr(param.find('=') + 1);
        if (key == "Expires") {
            auto expires = static_cast<std::time_t>(std::stoll(value));
            result[key] = expires;
        }
        else if (key == "SESSDATA" || key == "bili_jct") {
            result[key] = value;
        }
    }
    updateData("sVanilla.data", "cookie", result);
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
