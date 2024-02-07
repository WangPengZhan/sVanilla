#pragma once

#include <string>
#include <unordered_map>
#include <map>

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

    void HttpGet(const std::string& url,  const ParamType& params, std::string& response, const std::string& cookie = "");
    void HttpGet(const std::string& url, const std::string& params, std::string& response);
    void HttpGet(const std::string& url, std::string& response);
    void HttpGetWithCookie(const std::string& url, std::string& response, const std::string& cookie);

    void HttpPost(const std::string& url, ParamType params, std::string& response);
    void HttpPost(const std::string& url, const std::string& params, std::string& response);
    void HttpPost(const std::string& url, std::string& response);

    void HttpPut(const std::string& url, ParamType params, std::string& response);
    void HttpPut(const std::string& url, std::string& response);

    void SetHeaders(curl_slist* headers);
    void AppendHeaders(curl_slist* headers);
    void AppendHeaders(const std::string& header);
    static std::string ConcatenateParams(const ParamType &params);
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
