
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

namespace network
{

constexpr char domain_key[] = "Domain";
constexpr char expires_key[] = "Expires";
constexpr char path_key[] = "Path";
constexpr char secure_value[] = "Secure";
constexpr char httpOnly_value[] = "HttpOnly";
constexpr char set_cookie_header[] = "Set-Cookie: ";
constexpr char set_cookies[] = "Set-Cookie";
constexpr char same_site[] = "SameSite";

class CurlCookie
{
public:
    using CookieValuaType = std::unordered_map<std::string, std::string>;
    enum SameSiteSend
    {
        Invalid,
        Strict,
        Lax,
        None,
    };
    static std::string toString(SameSiteSend send);
    static SameSiteSend stringToSameSiteSend(const std::string& strSend);
    static bool isDefaultKey(std::string_view view);

    CurlCookie() = default;
    CurlCookie(const std::string& context);
    CurlCookie(std::string name, std::string value);

    CurlCookie& setName(const std::string& name);
    const std::string& name() const;
    CurlCookie& setValue(const std::string& name);
    const std::string& value() const;
    CurlCookie& setPath(const std::string& path);
    const std::string& path() const;
    CurlCookie& setDomain(const std::string& name);
    const std::string& domain() const;
    CurlCookie& setSecure(bool secure);
    bool secure() const;
    CurlCookie& setHttpOnly(bool httpOnly);
    bool httpOnly() const;
    CurlCookie& setExpireDataTime(const std::string& datatime);
    const std::string& expireDatatime() const;
    CurlCookie& setSameSite(SameSiteSend send = Invalid);
    SameSiteSend sameSite() const;

    void setContent(const std::string& context);
    std::string content(bool isContainSetCookie = false) const;
    operator std::string() const;

    bool contains(const std::string& key) const;
    const std::string& value(const std::string& key) const;
    std::vector<std::string> keys() const;

    static CurlCookie parseCookie(const std::string& content);

private:
    std::string m_name;
    CookieValuaType m_cookieValue;
    std::string m_empty;
};

}  // namespace network
