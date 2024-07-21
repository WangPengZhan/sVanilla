#include <filesystem>
#include <fstream>
#include <sstream>

#include "CurlCookie.h"
#include "CurlCookieJar.h"
#include "CurlEasy.h"

namespace network
{
CurlCookieJar::CurlCookieJar(std::string filename)
    : m_filename(std::move(filename))
{
    if (!m_filename.empty() && !std::filesystem::exists(m_filename))
    {
        std::ofstream stream(m_filename);
    }
}

CurlCookieJar::operator bool() const
{
    if (m_filename.empty())
    {
        return false;
    }

    return std::filesystem::exists(m_filename);
}

bool CurlCookieJar::contains(const std::string& key) const
{
    return toCookies().contains(key);
}

std::string CurlCookieJar::value(const std::string& key) const
{
    return toCookies().value(key);
}

std::vector<std::string> CurlCookieJar::keys() const
{
    return toCookies().keys();
}

void CurlCookieJar::setToCurl(CURL* handle)
{
    if (handle && this->operator bool())
    {
        curl_easy_setopt(handle, CURLOPT_COOKIEJAR, m_filename.c_str());
        curl_easy_setopt(handle, CURLOPT_COOKIEFILE, m_filename.c_str());
    }
}

void CurlCookieJar::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

CurlCookies CurlCookieJar::toCookies() const
{
    CurlCookies cookies;
    if (this->operator bool())
    {
        std::ifstream inputFile(m_filename);

        if (!inputFile)
        {
            return cookies;
        }

        std::stringstream buffer;
        buffer << inputFile.rdbuf();

        cookies.setContent(buffer.str());
    }

    return cookies;
}

}  // namespace network
