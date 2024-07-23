#include "CurlCookie.h"

namespace network
{

std::string CurlCookie::toString(SameSiteSend send)
{
    switch (send)
    {
    case Strict:
        return "Strict";
    case Lax:
        return "Lax";
    case None:
        return "None";
    case Invalid:
    default:
        break;
    }

    return {};
}

CurlCookie::SameSiteSend CurlCookie::stringToSameSiteSend(const std::string& strSend)
{
    if (strSend == "Strict")
    {
        return Strict;
    }
    else if (strSend == "Lax")
    {
        return Lax;
    }
    else if (strSend == "None")
    {
        return None;
    }

    return Invalid;
}

bool CurlCookie::isDefaultKey(std::string_view view)
{
    return (view == expires_key || view == path_key || view == domain_key || view == secure_value || view == httpOnly_value || view == same_site);
}

CurlCookie::CurlCookie(const std::string& context)
{
    *this = parseCookie(context);
}

CurlCookie::CurlCookie(std::string name, std::string value)
    : m_name(std::move(name))
{
    setValue(value);
}

CurlCookie& CurlCookie::setName(const std::string& name)
{
    m_name = name;
    return *this;
}

const std::string& CurlCookie::name() const
{
    return m_name;
}

CurlCookie& CurlCookie::setValue(const std::string& value)
{
    if (value.empty())
    {
        m_cookieValue.erase(m_name);
    }
    else
    {
        if (!m_name.empty())
        {
            m_cookieValue[m_name] = value;
        }
    }
    return *this;
}

const std::string& CurlCookie::value() const
{
    if (m_cookieValue.find(m_name) == m_cookieValue.end())
    {
        return m_empty;
    }

    return m_cookieValue.at(m_name);
}

CurlCookie& CurlCookie::setPath(const std::string& path)
{
    if (path.empty())
    {
        m_cookieValue.erase(path_key);
    }
    else
    {
        m_cookieValue[path_key] = path;
    }
    return *this;
}

const std::string& CurlCookie::path() const
{
    if (m_cookieValue.find(path_key) == m_cookieValue.end())
    {
        return m_empty;
    }

    return m_cookieValue.at(path_key);
}

CurlCookie& CurlCookie::setDomain(const std::string& domain)
{
    if (domain.empty())
    {
        m_cookieValue.erase(domain_key);
    }
    else
    {
        m_cookieValue[domain_key] = domain;
    }
    return *this;
}

const std::string& CurlCookie::domain() const
{
    if (m_cookieValue.find(domain_key) == m_cookieValue.end())
    {
        return m_empty;
    }

    return m_cookieValue.at(domain_key);
}

CurlCookie& CurlCookie::setSecure(bool secure)
{
    if (secure)
    {
        m_cookieValue[secure_value] = "";
    }
    else
    {
        m_cookieValue.erase(secure_value);
    }
    return *this;
}

bool CurlCookie::secure() const
{
    return m_cookieValue.find(secure_value) != m_cookieValue.end();
}

CurlCookie& CurlCookie::setHttpOnly(bool httpOnly)
{
    if (httpOnly)
    {
        m_cookieValue[httpOnly_value] = "";
    }
    else
    {
        m_cookieValue.erase(httpOnly_value);
    }

    return *this;
}

bool CurlCookie::httpOnly() const
{
    return m_cookieValue.find(httpOnly_value) != m_cookieValue.end();
}

CurlCookie& CurlCookie::setExpireDataTime(const std::string& datatime)
{
    if (datatime.empty())
    {
        m_cookieValue.erase(expires_key);
    }
    else
    {
        m_cookieValue[expires_key] = datatime;
    }

    return *this;
}

const std::string& CurlCookie::expireDatatime() const
{
    if (m_cookieValue.find(expires_key) == m_cookieValue.end())
    {
        return m_empty;
    }

    return m_cookieValue.at(expires_key);
}

CurlCookie& CurlCookie::setSameSite(SameSiteSend send)
{
    std::string strSend = toString(send);

    if (strSend.empty())
    {
        m_cookieValue.erase(same_site);
    }
    else
    {
        m_cookieValue[same_site] = strSend;
    }

    return *this;
}

CurlCookie::SameSiteSend CurlCookie::sameSite() const
{
    if (m_cookieValue.find(same_site) == m_cookieValue.end())
    {
        return Invalid;
    }

    return stringToSameSiteSend(m_cookieValue.at(same_site));
}

void CurlCookie::setContent(const std::string& context)
{
    *this = std::move(parseCookie(context));
}

std::string CurlCookie::content(bool isContainSetCookie) const
{
    std::string cookieString;
    cookieString.reserve(256);
    if (m_cookieValue.empty())
    {
        return "";
    }

    if (isContainSetCookie)
    {
        cookieString.append(set_cookie_header);
    }

    if (!m_name.empty() && m_cookieValue.find(m_name) != m_cookieValue.end())
    {
        cookieString.append(m_name);
        cookieString.append("=");
        cookieString.append(m_cookieValue.at(m_name));
        cookieString.append("; ");
    }

    for (const auto& valuePair : m_cookieValue)
    {
        if (valuePair.first == m_name)
        {
            continue;
        }

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
        if (!pair.second.empty())
        {
            ret.push_back(pair.first);
        }
    }

    return ret;
}

CurlCookie CurlCookie::parseCookie(const std::string& content)
{
    CurlCookie curlCookie;
    std::string_view content_view(content);
    if (content_view.starts_with(set_cookie_header))
    {
        content_view = std::string_view(content.data() + std::size(set_cookie_header) - 1);
    }

    bool hasName = false;
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
        }
        else
        {
            key = parseContent;
        }

        if (!isDefaultKey(key) && !hasName)
        {
            curlCookie.m_name = key;
            hasName = true;
        }

        curlCookie.m_cookieValue.insert(std::make_pair(key, value));
    }

    return curlCookie;
}

}  // namespace network
