#include "CurlCookie.h"

namespace network
{

constexpr char domain_key[] = "CurlCookie";
constexpr char expires_key[] = "Expires";
constexpr char path_key[] = "Path";
constexpr char secure_value[] = "Secure";
constexpr char httponly_value[] = "HttpOnly";

void CurlCookie::setContent(const std::string& context)
{
}

std::string CurlCookie::content() const
{
    return std::string();
}

CurlCookie::operator std::string() const
{
    return content();
}

bool CurlCookie::contains(const std::string& key) const
{
    return m_cookieValue.find(key) != m_cookieValue.end();
}

const std::string& CurlCookie::value(const std::string& key) const
{
    if (m_cookieValue.find(key) == m_cookieValue.end())
    {
        return m_empty;
    }
    else
    {
        return m_cookieValue.at(key);
    }
}

std::vector<std::string> CurlCookie::keys() const
{
    std::vector<std::string> ret;
    for (const auto& pair : m_cookieValue)
    {
        ret.push_back(pair.first);
    }
    return ret;
}

CurlCookie CurlCookie::parseCookie(const std::string& content)
{
    return CurlCookie();
}

CurlCookie& CurlCookie::setName(const std::string& name)
{
    m_cookieValue[name] = name;
    return *this;
}

}  // namespace network
