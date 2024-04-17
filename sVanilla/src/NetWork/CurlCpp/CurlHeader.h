#pragma once
#include <initializer_list>
#include <string>
#include <memory>
#include <vector>

#include <curl/curl.h>

namespace network
{

class CurlHeader
{
public:
    struct Deleter
    {
        void operator()(curl_slist* list);
    };
    using CurlListPtr = std::unique_ptr<curl_slist, Deleter>;

    CurlHeader();
    CurlHeader(std::initializer_list<std::string> headers);
    ~CurlHeader();

    CurlHeader(const CurlHeader& other);
    CurlHeader& operator=(const CurlHeader& other);
    CurlHeader(CurlHeader&& other);
    CurlHeader& operator=(CurlHeader&& other);

    void add(const std::string& header);
    template <typename Iterator>
    void add(Iterator begin, Iterator end);

    const curl_slist* get() const;
    curl_slist* handle();
    operator std::vector<std::string>() const;

private:
    CurlListPtr m_headers;
};

template <typename Iterator>
void CurlHeader::add(Iterator begin, Iterator end)
{
    auto header = m_headers.get();
    for (; begin < end; ++begin)
    {
        header = curl_slist_append(header, (*begin).c_str());
    }
    m_headers.release();
    m_headers.reset(header);
}

}  // namespace network
