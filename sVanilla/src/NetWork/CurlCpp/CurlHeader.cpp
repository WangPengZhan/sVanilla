#include "CurlHeader.h"

namespace network
{

void CurlHeader::Deleter::operator()(curl_slist* list)
{
    if (list)
    {
        curl_slist_free_all(list);
    }
}

CurlHeader::CurlHeader(std::initializer_list<std::string> headers)
{
    curl_slist* newHeader = nullptr;
    for (const auto& header : headers)
    {
        newHeader = curl_slist_append(newHeader, header.c_str());
    }
    m_headers.reset(newHeader);
}

CurlHeader::~CurlHeader()
{
    
}

CurlHeader::CurlHeader(const CurlHeader& other)
{
    curl_slist* otherPtr = other.m_headers.get();
    curl_slist* newHeader = nullptr;

    while (otherPtr != nullptr)
    {
        newHeader = curl_slist_append(newHeader, otherPtr->data);
        otherPtr = otherPtr->next;
    }

    m_headers.reset(newHeader);
}

CurlHeader& CurlHeader::operator=(const CurlHeader& other)
{
    if (this == &other)
    {
        return *this;
    }

    curl_slist* otherPtr = other.m_headers.get();
    curl_slist* newHeader = nullptr;

    while (otherPtr != nullptr)
    {
        newHeader = curl_slist_append(newHeader, otherPtr->data);
        otherPtr = otherPtr->next;
    }

    m_headers.reset(newHeader);
    return *this;
}

CurlHeader::CurlHeader(CurlHeader&& other)
{
    m_headers = std::move(other.m_headers);
}

CurlHeader& CurlHeader::operator=(CurlHeader&& other)
{
    m_headers = std::move(other.m_headers);
    return *this;
}

void CurlHeader::add(const std::string& header)
{
    auto newHeaders = curl_slist_append(m_headers.get(), header.c_str());
    if (!newHeaders)
    {

    }

    m_headers.release();
    m_headers.reset(newHeaders);
}

const curl_slist* CurlHeader::get() const
{
    return m_headers.get();
}

curl_slist* CurlHeader::handle()
{
    return m_headers.get();
}

CurlHeader::operator std::vector<std::string>() const
{
    std::vector<std::string> res;
    auto handle = get();
    while (handle)
    {
        res.emplace_back(handle->data);
        handle = handle->next;
    }

    return res;
}

}  // namespace network
