
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include <curl/curl.h>

namespace network
{
class CurlCookie;

class CurlEasy;

class CurlCookies
{
public:
    using CurlCookiesValuaType = std::unordered_map<std::string, std::string>;

    CurlCookies() = default;
    CurlCookies(const std::string& context);

    void setContent(const std::string& context);
    std::string content() const;
    operator std::string() const;

    CurlCookies& addContext(const std::string& key, const std::string& value);
    CurlCookies& addContext(const std::string& context);
    CurlCookies& addCurlCookie(const CurlCookie& cookie);
    CurlCookies& addCurlCookies(const CurlCookies& cookieJar);

    bool contains(const std::string& key) const;
    const std::string& value(const std::string& key) const;
    std::vector<std::string> keys() const;

    void setToCurl(CURL* handle) const;
    void setToCurl(CurlEasy& easy) const;

    static CurlCookies parseCookie(const std::string& content);

private:
    CurlCookiesValuaType m_cookieValue;
    std::string m_empty;
};

}  // namespace network
