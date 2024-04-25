#pragma once
#include "CNetWork.h"

namespace network
{
template <typename Response>
inline bool NetWork::request(const std::string& url, Response& response, HttpMethod medthod)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    std::string method = to_string(medthod);
    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), true);
    writer.setToCurl(easy);

    easy.perform();

    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::request(const std::string& url, Response& response, HttpMethod medthod, const CurlHeader& headers, bool headersAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    std::string method = to_string(medthod);
    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, CurlOptions(), true);
    writer.setToCurl(easy);

    easy.perform();

    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::request(const std::string& url, Response& response, HttpMethod medthod, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    std::string method = to_string(medthod);
    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, options, optionsAdd);
    writer.setToCurl(easy);

    easy.perform();

    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::request(const std::string& url, Response& response, HttpMethod medthod, const CurlHeader& headers, bool headersAdd,
                             const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    std::string method = to_string(medthod);
    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, method.c_str());
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, options, optionsAdd);
    writer.setToCurl(easy);

    easy.perform();

    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response)
{
    return request(url, response, HttpMethod::GET);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    return request(url, response, HttpMethod::GET, headers, headersAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd)
{
    return request(url, response, HttpMethod::GET, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd)
{
    return request(url, response, HttpMethod::GET, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params)
{
    std::string split = (url.find('?') == std::string::npos) ? "?" : "&";
    std::string paramUrl = url + split + paramsString(params);
    return request(url, response, HttpMethod::GET);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd)
{
    std::string split = (url.find('?') == std::string::npos) ? "?" : "&";
    std::string paramUrl = url + split + paramsString(params);
    return get(paramUrl, response, headers, headersAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlOptions& options, bool optionsAdd)
{
    std::string split = (url.find('?') == std::string::npos) ? "?" : "&";
    std::string paramUrl = url + split + paramsString(params);
    return get(paramUrl, response, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd,
                         const CurlOptions& options, bool optionsAdd)
{
    std::string split = (url.find('?') == std::string::npos) ? "?" : "&";
    std::string paramUrl = url + split + paramsString(params);
    return get(paramUrl, response, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response)
{
    return request(url, response, HttpMethod::POST);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    return request(url, response, HttpMethod::POST, headers, headersAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd)
{
    return request(url, response, HttpMethod::POST, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd)
{
    return request(url, response, HttpMethod::POST, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const std::string& params)
{
    CurlOptions options;
    auto postFields = std::make_shared<PostFields>(params);
    options.insert({postFields->getOption(), postFields});
    return post(url, response);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const std::string& params, const CurlHeader& headers, bool headersAdd)
{
    CurlOptions options;
    auto postFields = std::make_shared<PostFields>(params);
    options.insert({postFields->getOption(), postFields});
    return post(url, response, headers, headersAdd, options, true);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const std::string& params, const CurlOptions& options, bool optionsAdd)
{
    CurlOptions newOptions = options;
    auto postFields = std::make_shared<PostFields>(params);
    newOptions.insert({postFields->getOption(), postFields});
    return post(url, response, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const std::string& params, const CurlHeader& headers, bool headersAdd,
                          const CurlOptions& options, bool optionsAdd)
{
    CurlOptions newOptions = options;
    auto postFields = std::make_shared<PostFields>(params);
    newOptions.insert({postFields->getOption(), postFields});
    return post(url, response, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::head(const std::string& url, Response& response)
{
    return request(url, response, HttpMethod::HEAD);
}

template <typename Response>
inline bool NetWork::head(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    return request(url, response, HttpMethod::HEAD, headers, headersAdd);
}
}  // namespace network
