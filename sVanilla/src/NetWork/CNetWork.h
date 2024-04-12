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

    const CurlHeader& commonHeaders() const;
    void setCommonHeaders(const CurlHeader& commonsHeaders);
    const CurlOptions& commonOptions() const;
    void setCommonOptions(const CurlOptions& options);

    void addCommonOption(std::shared_ptr<AbstractOption> option);
    void addCommonOption(const std::vector<std::shared_ptr<AbstractOption>>& options);
    std::shared_ptr<AbstractOption> getOption(CURLoption opt) const;

protected:
private:
protected:
    CurlHeader m_commonHeaders;
    CurlOptions m_commonOptions;

private:
};

}  // namespace network
