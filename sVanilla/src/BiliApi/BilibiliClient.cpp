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
#include "BilibiliLog.h"
#include "BilibiliUtils.h"
#include "NetWork/NetworkLog.h"
#include "Util/UrlProcess.h"

namespace biliapi
{
static constexpr int Seconds = 60;
static constexpr int Minutes = 60;
static constexpr int Hours = 24;
static constexpr int DaySeconds = Hours * Minutes * Seconds;  // 24*60*60

BilibiliClient::BilibiliClient()
    : m_logined(false)
{
    initDefaultHeaders();
    initDefaultOptions();
}

BilibiliClient& BilibiliClient::globalClient()
{
    static BilibiliClient bilibiliClient;
    return bilibiliClient;
}

VideoViewOrigin BilibiliClient::getVideoView(const std::string& bvid)
{
    ParamType param;
    param.emplace("bvid", bvid);

    std::string response;
    get(VideoURL::View, response, param);
    return VideoViewOrigin(getDataFromRespones(response));
}

PlayUrlOrigin BilibiliClient::getPlayUrl(long long cid, long long qn, const std::string& bvid)
{
    ParamType param;
    param.emplace("bvid", bvid);
    param.emplace("cid", std::to_string(cid));
    param.emplace("qn", std::to_string(qn));
    param.emplace("fnver", "0");
    param.emplace("fnval", "0");
    param.emplace("fourk", "1");

    encodeWithWbi(param);

    std::string response;
    get(VideoURL::Playurl, response, param);

    return PlayUrlOrigin(getDataFromRespones(response));
}

LoginUrlOrigin BilibiliClient::getLoginUrl()
{
    std::string response;
    get(PassportURL::QRCode, response, passPortHeaders());
    return LoginUrlOrigin(getDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::getLoginStatus(const std::string& qrcode_key)
{
    ParamType param;
    param["qrcode_key"] = qrcode_key;

    std::string response;
    get(PassportURL::LoginStatus, response, param, passPortHeaders());
    return LoginStatusScanning(getDataFromRespones(response));
}

void BilibiliClient::setLogined(bool logined)
{
    m_logined = logined;
}

bool BilibiliClient::isLogined() const
{
    return m_logined;
}

void BilibiliClient::resetWbi()
{
    std::string response;
    get(PassportURL::WebNav, response, network::CurlHeader(), false, CurlOptions(), false);

    const auto navData = Nav(getDataFromRespones(response));
    const auto key = navData.data.wbi_img;
    if (!key.img_url.empty() && !key.sub_url.empty())
    {
        const auto img_url = util::u8ToString(std::filesystem::u8path(key.img_url).stem().u8string());
        const auto sub_url = util::u8ToString(std::filesystem::u8path(key.sub_url).stem().u8string());
        nlohmann::json j;
        j.emplace("mixin_key", GetMixinKey(img_url + sub_url));
        j.emplace("Expires", (std::time(nullptr) + DaySeconds) / DaySeconds);  // 有效期一天
        updateData("mixinKey", j);
    }
}

void BilibiliClient::encodeWithWbi(ParamType& params)
{
    MixinKey mixinKey;
    if (!readData(mixinKey, "mixinKey"))
    {
        resetWbi();
        readData(mixinKey, "mixinKey");
    }
    // 添加 wts 字段
    const time_t curr_time = time(nullptr);
    params.emplace("wts", std::to_string(curr_time));

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
    params.emplace("w_rid", MD5Hash(query + mixinKey.mixin_key));
}

void BilibiliClient::parseCookie(const std::string& url)
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
            result.emplace("Expires", expires / DaySeconds);
        }
        else if (key == "SESSDATA" || key == "bili_jct")
        {
            result.emplace(key, value);
        }
    }
    updateData("cookie", result);
}

nlohmann::json BilibiliClient::getDataFromRespones(const std::string& respones)
{
    nlohmann::json json;
    try
    {
        json = nlohmann::json::parse(respones);
        util::JsonProcess::removeNullValues(json);
    }
    catch (std::exception& e)
    {
        // BILIBILI_LOG_ERROR("Error parsing response: ", e.what());
    }

    return json;
}

network::CurlHeader BilibiliClient::passPortHeaders()
{
    network::CurlHeader header;

    std::string userAgent = std::string("user-agent: ") + network::chrome;
    header.add(userAgent);
    header.add(Headers::PassportReferer);

    return header;
}

void BilibiliClient::initDefaultHeaders()
{
    std::string userAgent = std::string("user-agent: ") + network::chrome;
    m_commonHeaders.add(userAgent);
    m_commonHeaders.add(network::accept_language);
    m_commonHeaders.add(network::accept_encoding);
}

void BilibiliClient::initDefaultOptions()
{
    constexpr time_t timeoutSecond = 5000;
    auto timeout = std::make_shared<network::TimeOut>(timeoutSecond);
    m_commonOptions.insert({timeout->getOption(), timeout});
    auto acceptEncoding = std::make_shared<network::AcceptEncoding>("gzip");
    m_commonOptions.insert({acceptEncoding->getOption(), acceptEncoding});
    auto sslVerifyHost = std::make_shared<network::SSLVerifyHost>(false);
    m_commonOptions.insert({sslVerifyHost->getOption(), sslVerifyHost});
    auto sslVerifyPeer = std::make_shared<network::SSLVerifyPeer>(false);
    m_commonOptions.insert({sslVerifyPeer->getOption(), sslVerifyPeer});
    auto verbose = std::make_shared<network::Verbose>(false);
    m_commonOptions.insert({verbose->getOption(), verbose});
}

}  // namespace biliapi
