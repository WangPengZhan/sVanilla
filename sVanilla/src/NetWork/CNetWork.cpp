#include <curl/curl.h>
#include <nlohmann/json.hpp>

#include "CNetWork.h"
#include "NetworkLog.h"

namespace network
{

std::string to_string(HttpMethod method)
{
    static std::unordered_map<HttpMethod, std::string> methodMap = {
        {HttpMethod::DEL,     "DELETE" },
        {HttpMethod::PUT,     "PUT"    },
        {HttpMethod::GET,     "GET"    },
        {HttpMethod::HEAD,    "HEAD"   },
        {HttpMethod::POST,    "POST"   },
        {HttpMethod::PATCH,   "PATCH"  },
        {HttpMethod::OPTIONS, "OPTIONS"},
    };
    return methodMap[method];
}

const CurlHeader& NetWork::commonHeaders() const
{
    return m_commonHeaders;
}

void NetWork::setCommonHeaders(const CurlHeader& commonsHeaders)
{
    m_commonHeaders = commonsHeaders;
}

const NetWork::CurlOptions& NetWork::commonOptions() const
{
    return m_commonOptions;
}

void NetWork::setCommonOptions(const CurlOptions& options)
{
    m_commonOptions = options;
}

void NetWork::addCommonOption(std::shared_ptr<AbstractOption> option)
{
    m_commonOptions.insert({option->getOption(), option});
}

void NetWork::addCommonOption(const std::vector<std::shared_ptr<AbstractOption>>& options)
{
    for (const auto& option : options)
    {
        addCommonOption(option);
    }
}

std::shared_ptr<AbstractOption> NetWork::getOption(CURLoption opt) const
{
    if (m_commonOptions.find(opt) != m_commonOptions.end())
    {
        return m_commonOptions.at(opt);
    }
    else
    {
        return std::shared_ptr<AbstractOption>();
    }
}

std::string NetWork::paramsString(const ParamType& params)
{
    std::string res;
    for (auto it = params.begin(); it != params.end(); ++it)
    {
        if (it != params.begin())
        {
            res += "&";
            res += it->first;
            res += "=";
            res += it->second;
        }
        else
        {
            res += it->first;
            res += "=";
            res += it->second;
        }
    }
    return res;
}

CurlHeader NetWork::setToCurl(CurlEasy& easy, const CurlHeader& headers, bool headersAdd)
{
    if (headersAdd)
    {
        CurlHeader headersCopy(headers);
        auto common = std::vector<std::string>(m_commonHeaders);
        headersCopy.add(common.begin(), common.end());
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headersCopy.get());
        return headersCopy;
    }
    else
    {
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headers.get());
        return headers;
    }
}

void NetWork::setToCurl(CurlEasy& easy, const CurlOptions& options, bool optionsAdd)
{
    for (const auto& option : options)
    {
        option.second->setToCurl(easy.handle());
    }

    if (optionsAdd)
    {
        for (const auto& option : m_commonOptions)
        {
            option.second->setToCurl(easy.handle());
        }
    }
}

}  // namespace network
