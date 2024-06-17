
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

namespace network
{

class CurlCookie
{
public:
    using CookieValuaType = std::unordered_map<std::string, std::string>;

    CurlCookie& setName(const std::string& name);
    const std::string& name() const;
    CurlCookie& setValue(const std::string& name);
    const std::string& value() const;
    CurlCookie& setPath(const std::string& name);
    const std::string& path() const;
    CurlCookie& setDomain(const std::string& name);
    const std::string& domain() const;
    CurlCookie& setSource(bool source);
    bool source() const;
    CurlCookie& setDataTime(const std::string& datatime);
    const std::string& datatime() const;

    void setContent(const std::string& context);
    std::string content() const;
    operator std::string() const;

    bool contains(const std::string& key) const;
    const std::string& value(const std::string& key) const;
    std::vector<std::string> keys() const;

    static CurlCookie parseCookie(const std::string& content);

private:
    CookieValuaType m_cookieValue;
    std::string m_empty;
};

}  // namespace network
