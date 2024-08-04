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
#include "NetWork/HeaderBodyResponseWrapper.h"
#include "NetWork/CurlCpp/CurlCookie.h"

namespace biliapi
{
static constexpr int seconds = 60;
static constexpr int minutes = 60;
static constexpr int hours = 24;
static constexpr int daySeconds = hours * minutes * seconds;  // 24*60*60

constexpr char cookie_dedeUserId[] = "DedeUserID";
constexpr char cookie_biliJct[] = "bili_jct";
constexpr char cookie_sessdata[] = "SESSDATA";

std::string to_string(BusinessType businessType)
{
    switch (businessType)
    {
    case biliapi::BusinessType::Archive:
        return "archive";
    case biliapi::BusinessType::Pgc:
        return "pgc";
    case biliapi::BusinessType::Live:
        return "live";
    case biliapi::BusinessType::Article_list:
        return "article-list";
    case biliapi::BusinessType::Article:
        return "article";
    case biliapi::BusinessType::Unkown:
    default:
        break;
    }

    return std::string();
}

std::string to_string(HistoryType historyType)
{
    switch (historyType)
    {
    case biliapi::All:
        return "all";
    case biliapi::Archive:
        return "archive";
    case biliapi::Live:
        return "live";
    case biliapi::Article:
        return "article";
    case biliapi::Unkown:
    default:
        break;
    }
    return std::string();
}

network::NetWork::ParamType HistoryQueryParam::toParam() const
{
    network::NetWork::ParamType res;
    if (max != 0)
    {
        res.insert({"max", std::to_string(max)});
    }

    if (business != BusinessType::Unkown)
    {
        res.insert({"business", to_string(business)});
    }

    if (view_at != 0)
    {
        res.insert({"view_at", std::to_string(view_at)});
    }

    if ((type != HistoryType::Unkown && type != HistoryType::All))
    {
        res.insert({"type", to_string(type)});
    }

    if (ps != 0 && ps != 20)
    {
        res.insert({"ps", std::to_string(ps)});
    }

    return res;
}

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

    try
    {
        return PlayUrlOrigin(getDataFromRespones(response));
    }
    catch (const std::exception& e)
    {
        std::string str = e.what();
        return PlayUrlOrigin();
    }
}

LoginUrlOrigin BilibiliClient::getLoginUrl()
{
    std::string response;
    get(PassportURL::QRCode, response, passPortHeaders());
    return LoginUrlOrigin(getDataFromRespones(response));
}

LoginStatusScanning BilibiliClient::getLoginStatus(const std::string& qrcodeKey)
{
    ParamType param;
    param["qrcode_key"] = qrcodeKey;

    network::ResponseHeaderAndBody response;
    get(PassportURL::LoginStatus, response, param, passPortHeaders());
    auto header = parseHeader(response.header);
    if (header.end() != header.find(network::set_cookies))
    {
        std::lock_guard lk(m_mutexRequest);
        m_cookies.setContent(header.at(network::set_cookies));
        m_commonOptions[network::CookieFileds::opt] = std::make_shared<network::CookieFileds>(m_cookies);
    }

    return LoginStatusScanning(getDataFromRespones(response.body));
}

Nav BilibiliClient::getNavInfo()
{
    std::string response;
    get(PassportURL::WebNav, response, network::CurlHeader(), true, CurlOptions(), true);

    return Nav(getDataFromRespones(response));
}

LogoutExitV2 BilibiliClient::getLogoutExitV2()
{
    std::string response;

    auto keys = m_cookies.keys();
    int haveId = 3;

    for (const auto& key : keys)
    {
        if (key == cookie_dedeUserId || key == cookie_biliJct || key == cookie_sessdata)
        {
            --haveId;
        }
    }

    if (haveId == 0)
    {
        std::string cookie = "Cookie: ";
        cookie += std::string(cookie_dedeUserId) + "=" + m_cookies.value(cookie_dedeUserId);
        cookie += "; ";
        cookie += std::string(cookie_biliJct) + "=" + m_cookies.value(cookie_biliJct);
        cookie += "; ";
        cookie += std::string(cookie_sessdata) + "=" + m_cookies.value(cookie_sessdata);

        network::CurlHeader header;
        header.add("Content-Type: application/x-www-form-urlencoded");
        header.add(cookie);

        std::string param = "biliCSRF=" + m_cookies.value(cookie_biliJct);

        post(PassportURL::Logout, response, param, header, false);
    }

    auto logout = LogoutExitV2(getDataFromRespones(response));
    if (logout.code == 0)
    {
        std::lock_guard lk(m_mutexRequest);
        m_cookies = network::CurlCookies();
        m_commonOptions.erase(network::CookieFileds::opt);
    }
}

History BilibiliClient::getHistory(HistoryQueryParam param)
{
    std::string response;

    auto params = param.toParam();
    get(PassportURL::History, response, params);
    std::cout << response;
    return History(getDataFromRespones(response));
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
    const auto navData = getNavInfo();
    const auto key = navData.data.wbi_img;
    if (!key.img_url.empty() && !key.sub_url.empty())
    {
        const auto img_url = std::filesystem::u8path(key.img_url).stem().string();
        const auto sub_url = std::filesystem::u8path(key.sub_url).stem().string();
        nlohmann::json j;
        j.emplace("mixin_key", GetMixinKey(img_url + sub_url));
        j.emplace("Expires", (std::time(nullptr) + daySeconds) / daySeconds);  // 有效期一天
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
            result.emplace("Expires", expires / daySeconds);
        }
        else if (key == cookie_sessdata || key == cookie_biliJct)
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
        // BILIBILI_LOG_ERROR("Error parsing response: {}", e.what());
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
