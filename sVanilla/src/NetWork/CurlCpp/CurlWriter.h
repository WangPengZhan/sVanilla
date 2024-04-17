#pragma once
#include <string>

#include "CurlEasy.h"

namespace network
{

template <typename Context>
size_t writeFunc(void* data, size_t size, size_t nmemb, void* stream);

template <>
size_t writeFunc<std::string>(void* data, size_t size, size_t nmemb, void* stream);

template <typename Context>
class CurlWriter
{
public:
    CurlWriter(Context& ctx);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    void isValid() const;

private:
    Context& m_context;
};

template <typename Response>
class CurlResponseWrapper
{
public:
    CurlResponseWrapper(Response& response);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    void readAfter(CURL* handle);
    void readAfter(CurlEasy& easy);

private:
    Response& m_response;
    CurlWriter<Response> m_writer;
};

}  // namespace network

#include "CurlWriter.inl"
