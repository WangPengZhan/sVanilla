#include <curl/curl.h>

#include <nlohmann/json.hpp>

#include "CNetWork.h"
#include "NetworkLog.h"

namespace
{

size_t OnWriteDate(void* data, size_t size, size_t nmemb, void* stream)
{
    if (!stream)
    {
        return 0;
    }

    std::string* str = static_cast<std::string*>(stream);
    (*str).append(static_cast<char*>(data), size * nmemb);
    return size * nmemb;
}

constexpr char chrome[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36";
constexpr char firefox[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:68.0) Gecko/20100101 Firefox/68.0";
constexpr char edge[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299";
constexpr char opera[] =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 OPR/45.0.2552.8 (EdgE)";
constexpr char ie[] = "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 Edge/16.16299";
constexpr char safari[] =
    "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/58.0.3029.110 Safari/537.36 OPR/45.0.2552.8 (EdgE)";

constexpr char const accept[] = "Accept: application/json";
constexpr char const accept_encoding[] = "Accept-Encoding: gzip, deflate, br";
constexpr char const accept_language[] = "Accept-Language: zh-CN,zh;q=0.9,en-US;q=0.8,en;q=0.7";
constexpr char const connect_type[] = "Connection: keep-alive";

}  // namespace

CNetWork::CurlHelp::CurlHelp()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

CNetWork::CurlHelp::~CurlHelp()
{
    curl_global_cleanup();
}

CNetWork::CurlHelp CNetWork::m_curlHelp;

CNetWork::CNetWork() : m_headers(nullptr)
{
    InitDefaultHeaders();
}

CNetWork::~CNetWork()
{
    curl_slist_free_all(m_headers);
}

void CNetWork::SetHeaders(curl_slist* headers)
{
    if (m_headers)
    {
        curl_slist_free_all(m_headers);
    }

    m_headers = headers;
}

void CNetWork::AppendHeaders(curl_slist* headers)
{
    if (headers)
    {
        do
        {
            curl_slist_append(m_headers, headers->data);
            headers = headers->next;
        } while (headers->next);

        curl_slist_free_all(headers);
        headers = nullptr;
    }
}

void CNetWork::AppendHeaders(const std::string& header)
{
    curl_slist_append(m_headers, header.c_str());
}

void CNetWork::HttpGet(const std::string& url, ParamType params, std::string& response)
{
    std::string strParam;
    for (const auto& param : params)
    {
        strParam += param.first + "=" + param.second + "&";
    }

    if (std::string::npos == url.find("?"))
    {
        strParam = "?" + strParam;
    }

    strParam.erase(strParam.end() - 1);

    return HttpGet(url, strParam, response);
}

void CNetWork::HttpGet(const std::string& url, const std::string& params, std::string& response)
{
    return HttpGet(url + params, response);
}

void CNetWork::HttpGet(const std::string& url, std::string& response)
{
    CURL* curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "GET");
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, m_headers);
    curl_easy_setopt(curlHandle, CURLOPT_HEADER, false);
    curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, OnWriteDate);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 5000);
    curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, false);
    CURLcode retCode = curl_easy_perform(curlHandle);
    if (retCode != CURLE_OK)
    {
        //        NETWORK_LOG_ERROR("HttpGet occurred error: {}", int(retCode));
    }
    curl_easy_cleanup(curlHandle);
}

void CNetWork::HttpPost(const std::string& url, ParamType params, std::string& response)
{
    // std::list<std::string> listParams;
    // for (const auto& param : params)
    //{
    //     listParams.emplace_back(param.first + " " + param.second);
    // }

    nlohmann::json jsonParam = params;

    HttpPost(url, jsonParam.dump(), response);
}

void CNetWork::HttpPost(const std::string& url, const std::string& params, std::string& response)
{
    CURL* curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, m_headers);
    curl_easy_setopt(curlHandle, CURLOPT_HEADER, false);
    curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, OnWriteDate);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 5);
    curl_easy_setopt(curlHandle, CURLOPT_POSTFIELDS, params.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, false);
    curl_easy_setopt(curlHandle, CURLOPT_COOKIEFILE, "cookie.txt");
    CURLcode retCode = curl_easy_perform(curlHandle);

    if (retCode != CURLE_OK)
    {
        //        NETWORK_LOG_ERROR("HttpPost occured error: {}", int(retCode));
    }
    curl_easy_cleanup(curlHandle);
}

void CNetWork::HttpPost(const std::string& url, std::string& response)
{
    CURL* curlHandle = curl_easy_init();
    curl_easy_setopt(curlHandle, CURLOPT_CUSTOMREQUEST, "POST");
    curl_easy_setopt(curlHandle, CURLOPT_HTTPHEADER, m_headers);
    curl_easy_setopt(curlHandle, CURLOPT_HEADER, false);
    curl_easy_setopt(curlHandle, CURLOPT_VERBOSE, 0);
    curl_easy_setopt(curlHandle, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curlHandle, CURLOPT_WRITEFUNCTION, OnWriteDate);
    curl_easy_setopt(curlHandle, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curlHandle, CURLOPT_TIMEOUT, 5);
    curl_easy_setopt(curlHandle, CURLOPT_ACCEPT_ENCODING, "gzip");
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYPEER, false);
    curl_easy_setopt(curlHandle, CURLOPT_SSL_VERIFYHOST, false);
    CURLcode retCode = curl_easy_perform(curlHandle);
    if (retCode != CURLE_OK)
    {
//        NETWORK_LOG_ERROR("HttpPost occured error: {}", int(retCode));

    }
    curl_easy_cleanup(curlHandle);
}

void CNetWork::InitDefaultHeaders()
{
    std::string userAgent = std::string("user-agent: ") + chrome;
    m_headers = curl_slist_append(m_headers, accept_encoding);
    m_headers = curl_slist_append(m_headers, accept_language);
    m_headers = curl_slist_append(m_headers, userAgent.c_str());
}

void CNetWork::InitDefaultHeadersLogin()
{
    std::string userAgent = std::string("user-agent: ") + chrome;
    std::string referer = "referer: https://passport.bilibili.com/login";
    m_headers = curl_slist_append(m_headers, accept_language);
    m_headers = curl_slist_append(m_headers, userAgent.c_str());
    m_headers = curl_slist_append(m_headers, referer.c_str());
}
