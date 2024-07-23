#pragma once
#include <string>
#include <map>
#include <iostream>
#include <list>
#include <unordered_map>

#include "NetWork/CurlCpp/CurlHeader.h"
#include "NetWork/CurlCpp/CurlOption.h"
#include "NetWork/CurlCpp/CurlWriter.h"

namespace network
{

inline const std::string chrome = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36";
inline const std::string firefox = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:68.0) Gecko/20100101 Firefox/68.0";
inline const std::string edge =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299";
inline const std::string opera =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 OPR/45.0.2552.8 (EdgE)";
inline const std::string ie =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299";
inline const std::string safari =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 OPR/45.0.2552.8 (EdgE)";

inline const std::string accept = "Accept: application/json";
inline const std::string accept_encoding = "Accept-Encoding: gzip, deflate, br";
inline const std::string accept_language = "Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7";
inline const std::string connect_type = "Connection: keep-alive";

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

std::string to_string(HttpMethod method);

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

    // request for all
    template <typename Response>
    bool request(const std::string& url, Response& response, HttpMethod medthod = HttpMethod::GET);
    template <typename Response>
    bool request(const std::string& url, Response& response, HttpMethod medthod, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool request(const std::string& url, Response& response, HttpMethod medthod, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool request(const std::string& url, Response& response, HttpMethod medthod, const CurlHeader& headers, bool headersAdd, const CurlOptions& options,
                 bool optionsAdd = false);

    // get
    template <typename Response>
    bool get(const std::string& url, Response& response);
    template <typename Response>
    bool get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd, const CurlOptions& options,
             bool optionsAdd = false);

    // post
    template <typename Response>
    bool post(const std::string& url, Response& response);
    template <typename Response>
    bool post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool post(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool post(const std::string& url, Response& response, const std::string& params);
    template <typename Response>
    bool post(const std::string& url, Response& response, const std::string& params, const CurlHeader& headers, bool headersAdd = false);
    template <typename Response>
    bool post(const std::string& url, Response& response, const std::string& params, const CurlOptions& options, bool optionsAdd = false);
    template <typename Response>
    bool post(const std::string& url, Response& response, const std::string& params, const CurlHeader& headers, bool headersAdd, const CurlOptions& options,
              bool optionsAdd = false);

    // head
    template <typename Response>
    bool head(const std::string& url, Response& response);
    template <typename Response>
    bool head(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd = false);

    // util
    static std::string paramsString(const ParamType& params);
    static ParamType parseHeader(const std::string& header);

private:
    CurlHeader setToCurl(CurlEasy& easy, const CurlHeader& headers, bool headersAdd = false);
    void setToCurl(CurlEasy& easy, const CurlOptions& options, bool optionsAdd = false);

protected:
    CurlHeader m_commonHeaders;
    CurlOptions m_commonOptions;
};

}  // namespace network

#include "CNetWork.inl"
