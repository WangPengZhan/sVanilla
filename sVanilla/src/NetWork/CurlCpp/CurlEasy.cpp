#include "CurlEasy.h"


namespace network
{

void CurlEasy::Deleter::operator()(CURL* curl)
{
    if (curl)
    {
        curl_easy_cleanup(curl);
    }
}

CurlEasy::CurlEasy(CURL* handle)
{
    m_curl.reset(handle);
}

CurlEasy::~CurlEasy()
{
}

void CurlEasy::perform()
{
    const CURLcode code = curl_easy_perform(handle());
}

void CurlEasy::reset()
{
    curl_easy_reset(handle());
}

CURL* CurlEasy::handle() const
{
    return m_curl.get();
}

}  // namespace network