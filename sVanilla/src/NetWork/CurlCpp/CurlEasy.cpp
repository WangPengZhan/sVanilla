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

CurlEasy::CurlEasy()
{
    m_curl.reset(curl_easy_init());
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

void CurlEasy::setOption(const AbstractOption& option)
{
    setOption(&option);
}

void CurlEasy::setOption(std::unique_ptr<AbstractOption> option)
{
    setOption(option.get());
}

void CurlEasy::setOption(const AbstractOption* option)
{
    if (option)
    {
        option->setToCurl(handle());
    }
}

CURL* CurlEasy::handle() const
{
    return m_curl.get();
}

}  // namespace network