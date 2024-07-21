
#pragma once
#include <unordered_map>
#include <string>
#include <vector>

#include "CurlCookies.h"

namespace network
{
struct CURL;
class CurlEasy;

class CurlCookieJar
{
public:
    using CurlCookieJarValuaType = std::unordered_map<std::string, std::string>;

    CurlCookieJar() = default;
    CurlCookieJar(std::string filename);

    operator bool() const;
    operator std::string() const;

    bool contains(const std::string& key) const;
    std::string value(const std::string& key) const;
    std::vector<std::string> keys() const;

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    CurlCookies toCookies() const;

private:
    std::string m_filename;
};

}  // namespace network
