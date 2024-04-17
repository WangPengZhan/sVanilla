#pragma once
#include <string>

#include "CurlWriter.h"

namespace network
{

template <typename Context>
size_t writeFunc(void* data, size_t size, size_t nmemb, void* stream)
{
    const size_t realsize = size * nmemb;
    Context const ctx = static_cast<Context*>(stream);
    ctx->write(static_cast<const char*>(data), realsize);
    return realsize;
}

template <>
size_t writeFunc<std::string>(void* data, size_t size, size_t nmemb, void* stream)
{
    const size_t realsize = size * nmemb;
    std::string* const ctx = static_cast<std::string*>(stream);
    ctx->append(static_cast<const char*>(data), realsize);
    return realsize;
}

}  // namespace network
