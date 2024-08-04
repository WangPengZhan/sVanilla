#include "CurlCookies.h"
#include "CurlCookie.h"
#include "CurlEasy.h"

namespace network
{

CurlCookies::CurlCookies(const std::string& context)
{
    setContent(context);
}

void CurlCookies::setContent(const std::string& context)
{
    *this = std::move(parseCookie(context));
}

std::string CurlCookies::content() const
{
    std::string cookieString;
    cookieString.reserve(256);
    if (m_cookieValue.empty())
    {
        return "";
    }

    for (const auto& valuePair : m_cookieValue)
    {
        cookieString.append(valuePair.first);
        if (!valuePair.second.empty())
        {
            cookieString.append("=");
            cookieString.append(valuePair.second);
        }
        cookieString.append("; ");
    }

    if (cookieString.size() > 2)
    {
        cookieString.resize(cookieString.size() - 2);
    }

    return cookieString;
}

CurlCookies::operator std::string() const
{
    return content();
}

CurlCookies& CurlCookies::addContext(const std::string& key, const std::string& value)
{
    if (!key.empty())
    {
        m_cookieValue[key] = value;
    }

    return *this;
}

CurlCookies& CurlCookies::addContext(const std::string& context)
{
    return addCurlCookies(parseCookie(context));
}

CurlCookies& CurlCookies::addCurlCookie(const CurlCookie& cookie)
{
    return addContext(cookie.name(), cookie.value());
}

CurlCookies& CurlCookies::addCurlCookies(const CurlCookies& cookieJar)
{
    for (const auto& valuePair : cookieJar.m_cookieValue)
    {
        m_cookieValue[valuePair.first] = valuePair.second;
    }

    return *this;
}

bool CurlCookies::contains(const std::string& key) const
{
    return m_cookieValue.find(key) != m_cookieValue.end();
}

const std::string& CurlCookies::value(const std::string& key) const
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

std::vector<std::string> CurlCookies::keys() const
{
    std::vector<std::string> ret;
    for (const auto& pair : m_cookieValue)
    {
        if (!pair.second.empty())
        {
            ret.push_back(pair.first);
        }
    }

    return ret;
}

void CurlCookies::setToCurl(CURL* handle) const
{
    auto strCookies = content();
    if (handle && !strCookies.empty())
    {
        curl_easy_setopt(handle, CURLOPT_COOKIE, strCookies.c_str());
    }
}

void CurlCookies::setToCurl(CurlEasy& easy) const
{
    setToCurl(easy.handle());
}

CurlCookies CurlCookies::parseCookie(const std::string& content)
{
    CurlCookies CurlCookies;
    std::string_view content_view(content);

    while (content_view.find("; ") != std::string_view::npos)
    {
        int pos = content_view.find("; ");
        auto parseContent = content_view.substr(0, pos);
        content_view = content_view.substr(pos + 2);
        std::string_view key;
        std::string_view value;
        if (parseContent.find("=") != std::string_view::npos)
        {
            int pos = parseContent.find("=");
            key = parseContent.substr(0, pos);
            value = parseContent.substr(pos + 1);
            CurlCookies.m_cookieValue.insert(std::make_pair(key, value));
        }
    }

    return CurlCookies;
}

}  // namespace network
