#pragma once

#include <string>

#include <QThread>
#include <QNetworkAccessManager>
#include <QNetworkReply>

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

    using ParamType = std::unordered_map<std::string, std::string>;

public:
    CNetWork();
    ~CNetWork();

    void HttpGet(const std::string& url, ParamType params, std::string& response);
    void HttpGet(const std::string& url, const std::string& params, std::string& response);
    void HttpGet(const std::string& url, std::string& response);

    void HttpPost(const std::string& url, ParamType params, std::string& response);
    void HttpPost(const std::string& url, const std::string& params, std::string& response);
    void HttpPost(const std::string& url, std::string& response);

    void HttpPut(const std::string& url, ParamType params, std::string& response);
    void HttpPut(const std::string& url, std::string& response);

    void SetHeaders(curl_slist* headers);
    void AppendHeaders(curl_slist* headers);
    void AppendHeaders(const std::string& header);

protected:
    void InitDefaultHeaders();
    void InitDefaultHeadersLogin();

    curl_slist* m_headers;  // 请求头
    class CurlHelp
    {
    public:
        CurlHelp();
        ~CurlHelp();
    };

    static CurlHelp m_curlHelp;
};
