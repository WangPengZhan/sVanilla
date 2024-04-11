#pragma once
#include <memory>

#include <curl/curl.h>

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

    CurlEasy(const CurlEasy & other) = delete;
    CurlEasy& operator=(const CurlEasy& other) = delete;

    void perform();
    void reset();

    template<typename OptionType>
    void option(CURLoption option, OptionType value);

    template<typename OptionType, CURLoption optionType>
    void option(OptionType value);

    CURL* handle() const;

    template<typename T>
    void getInfo(CURLINFO info, T & value) const;

private:
    CURLPtr m_curl;
};

} // namespace network