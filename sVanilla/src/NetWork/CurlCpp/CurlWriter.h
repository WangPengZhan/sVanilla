#pragma once
#include <string>

#include "CurlEasy.h"

namespace network
{

template <typename Context>
class ContextWriter
{
    size_t operator()(void* data, size_t size, size_t nmemb, void* stream)
    {
        const size_t realsize = size * nmemb;
        Context* const ctx = static_cast<Context*>(stream);
        ctx->write(static_cast<const char*>(data), realsize);
        return realsize;
    }
};

template <>
class ContextWriter<std::string>
{
    size_t operator()(void* data, size_t size, size_t nmemb, void* stream)
    {
        const size_t realsize = size * nmemb;
        std::string* const ctx = static_cast<std::string*>(stream);
        ctx->append(static_cast<const char*>(data), realsize);
        return realsize;
    }
};

template <typename Context, typename Writer = ContextWriter<Context>>
class CurlWriter
{
public:
    CurlWriter(Context& ctx);
    CurlWriter(Context& ctx, Writer writer);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy* easy);

    void isValid() const;

private:
    Writer m_writer;
    Context& m_context;
};

template <typename Response>
class CurlResponseWrapper
{
public:
    CurlResponseWrapper(Response& response);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy* easy);

    void readAfter(CURL* handle);
    void readAfter(CurlEasy* easy);

private:
    Response& m_response;
    CurlWriter<Response> m_writer;
};

}  // namespace network

#include "CurlWriter.inl"
