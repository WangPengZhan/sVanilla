#pragma once
#include "CNetWork.h"

namespace network
{

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), true);
    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, CurlOptions(), true);
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), optionsAdd);

    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, options, optionsAdd);

    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd)
{
    std::string paramUrl = url + paramsString(params);
    return get(paramUrl, response, headers, headersAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlOptions& options, bool optionsAdd)
{
    std::string paramUrl = url + paramsString(params);
    return get(paramUrl, response, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::get(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd,
                         const CurlOptions& options, bool optionsAdd)
{
    std::string paramUrl = url + paramsString(params);
    return get(paramUrl, response, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), true);
    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, CurlOptions(), true);
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), optionsAdd);

    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd, const CurlOptions& options, bool optionsAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, options, optionsAdd);

    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd)
{
    std::string paramUrl = url + paramsString(params);
    return post(paramUrl, response, headers, headersAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const ParamType& params, const CurlOptions& options, bool optionsAdd)
{
    std::string paramUrl = url + paramsString(params);
    return post(paramUrl, response, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::post(const std::string& url, Response& response, const ParamType& params, const CurlHeader& headers, bool headersAdd,
                          const CurlOptions& options, bool optionsAdd)
{
    std::string paramUrl = url + paramsString(params);
    return post(paramUrl, response, headers, headersAdd, options, optionsAdd);
}

template <typename Response>
inline bool NetWork::head(const std::string& url, Response& response)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "HEAD");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, CurlHeader(), true);
    setToCurl(easy, CurlOptions(), true);
    easy.perform();
    writer.readAfter(easy);

    return true;
}

template <typename Response>
inline bool NetWork::head(const std::string& url, Response& response, const CurlHeader& headers, bool headersAdd)
{
    CurlEasy easy;
    CurlResponseWrapper writer(response);

    curl_easy_setopt(easy.handle(), CURLOPT_CUSTOMREQUEST, "HEAD");
    curl_easy_setopt(easy.handle(), CURLOPT_URL, url.c_str());
    setToCurl(easy, headers, headersAdd);
    setToCurl(easy, CurlOptions(), true);
    writer.setToCurl(easy);

    easy.perform();
    writer.readAfter(easy);

    return true;
}
}  // namespace network
