#pragma once
#include <string>
#include <map>
#include <iostream>
#include <list>
#define PRINT(x)       std::cout << "Variable: " << #x << ", Value: " << x << std::endl;
#define PRINTS(str, x) std::cout << str << ", Value: " << x << std::endl;
#define PRINTJ(str, x) std::cout << str << nlohmann::json::parse(x).dump(4) << std::endl;

struct curl_slist;

class CNetWork
{
public:
    enum HpptType
    {
        GET,
        POST,
        PUT,
    };

    using ParamType = std::map<std::string, std::string>;
    // using OrderParamType = std::map<std::string, std::string>;

public:
    CNetWork();
    ~CNetWork();

    // void HttpGet(const std::string& url,  const ParamType& params, std::string& response, const std::string& cookie = "");
    void HttpGet(const std::string& url, const ParamType& params, std::string& response, const std::list<std::string>& headers);
    void HttpGet(const std::string& url, std::string& response, const std::list<std::string>& headers);
    // void HttpGetWithCookie(const std::string& url, std::string& response, const std::string& cookie);
    void HttpGet(const std::string& url, FILE* file);

    void HttpPost(const std::string& url, ParamType params, std::string& response);
    void HttpPost(const std::string& url, const std::string& params, std::string& response);
    void HttpPost(const std::string& url, std::string& response);

    void HttpPut(const std::string& url, ParamType params, std::string& response);
    void HttpPut(const std::string& url, std::string& response);

    void SetHeaders(curl_slist* headers);
    void AppendHeaders(curl_slist* headers);
    void AppendHeaders(const std::string& header);
    static std::string ConcatenateParams(const ParamType& params);
    std::string getAgent();

protected:
    void InitDefaultHeaders();
    void InitDefaultHeadersLogin();

    curl_slist* m_headers;
    class CurlHelp
    {
    public:
        CurlHelp();
        ~CurlHelp();
    };

    static CurlHelp m_curlHelp;
};

#include <unordered_map>

#include "NetWork/CurlCpp/CurlHeader.h"
#include "NetWork/CurlCpp/CurlOption.h"
#include "NetWork/CurlCpp/CurlWriter.h"

namespace network
{

enum class HttpMethod
{
    DEL,
    PUT,
    GET,
    HEAD,
    POST,
    PATCH,
    OPTIONS
};

class NetWork
{
public:
    using CurlOptions = std::unordered_map<CURLoption, std::shared_ptr<AbstractOption>>;
    using ParamType = std::map<std::string, std::string>;

    NetWork() = default;
    virtual ~NetWork() = default;

    // headers
    const CurlHeader& commonHeaders() const;
    void setCommonHeaders(const CurlHeader& commonsHeaders);

    // options
    const CurlOptions& commonOptions() const;
    void setCommonOptions(const CurlOptions& options);
    void addCommonOption(std::shared_ptr<AbstractOption> option);
    void addCommonOption(const std::vector<std::shared_ptr<AbstractOption>>& options);
    std::shared_ptr<AbstractOption> getOption(CURLoption opt) const;

    template <typename Response>
    bool get(const std::string& url, Response& response);
    template <typename Response>
    bool get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd = false,
             const CurlOptions& options = {}, bool optionsAdd = false);

protected:
private:
protected:
    CurlHeader m_commonHeaders;
    CurlOptions m_commonOptions;

private:
};

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, m_commonHeaders.get());
    for (const auto& option : m_commonOptions)
    {
        option.second->setToCurl(easy);
    }
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    if (headersAdd)
    {
        CurlHeader headersCopy(headers);
        auto common = std::vector<std::string>(m_commonHeaders);
        headersCopy.add(common.begin(), common.end());
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headersCopy.get());
    }
    else
    {
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headers.get());
    }
    for (const auto& option : m_commonOptions)
    {
        option.second->setToCurl(easy);
    }
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, m_commonHeaders.get());

    for (const auto& option : options)
    {
        option.second->setToCurl(easy);
    }
    if (optionsAdd)
    {
        for (const auto& option : m_commonOptions)
        {
            option.second->setToCurl(easy);
        }
    }

    for (const auto& option : m_commonOptions)
    {
        option.second->setToCurl(easy);
    }
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd,
                         const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    if (headersAdd)
    {
        CurlHeader headersCopy(headers);
        auto common = std::vector<std::string>(m_commonHeaders);
        headersCopy.add(common.begin(), common.end());
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headersCopy.get());
    }
    else
    {
        curl_easy_setopt(easy.handle(), CURLOPT_HTTPHEADER, headers.get());
    }
    for (const auto& option : options)
    {
        option.second->setToCurl(easy);
    }
    if (optionsAdd)
    {
        for (const auto& option : m_commonOptions)
        {
            option.second->setToCurl(easy);
        }
    }

    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

}  // namespace network
