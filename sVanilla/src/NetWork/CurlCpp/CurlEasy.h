#pragma once
#include <memory>

#include <curl/curl.h>

#include "CurlOption.h"

namespace network
{

class CurlEasy
{
public:
    struct Deleter
    {
        void operator()(CURL* curl);
    };
    using CURLPtr = std::unique_ptr<CURL, Deleter>;

    CurlEasy();
    CurlEasy(CURL* handle);
    virtual ~CurlEasy();

    CurlEasy(const CurlEasy& other) = delete;
    CurlEasy& operator=(const CurlEasy& other) = delete;

    void perform();
    void reset();

    void setOption(const AbstractOption& option);
    void setOption(std::unique_ptr<AbstractOption> option);
    void setOption(const AbstractOption* option);
    template <typename CurlOption>
    void setOption(typename CurlOption::ValueType const& value);
    template <typename InputIterator>
    void setOption(InputIterator first, InputIterator last);

    CURL* handle() const;

    template <typename T>
    void getInfo(CURLINFO info, T& value) const;

private:
    CURLPtr m_curl;
};

}  // namespace network

#include "CurlEasy.inl"
