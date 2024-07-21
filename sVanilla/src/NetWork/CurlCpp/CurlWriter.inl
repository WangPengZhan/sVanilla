#pragma once
#include <functional>

namespace network
{
template <typename Context>
inline CurlWriter<Context>::CurlWriter(Context& ctx)
    : m_context(ctx)
{
}

template <typename Context>
inline void CurlWriter<Context>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeFunc<Context>);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &m_context);
    }
}

template <typename Context>
inline void CurlWriter<Context>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

template <typename Context>
inline bool CurlWriter<Context>::isValid() const
{
    return true;
}

template <typename Context>
inline CurlWriter<Context*>::CurlWriter(Context* ctx)
    : m_context(ctx)
{
}

template <typename Context>
inline void CurlWriter<Context*>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, writeFunc<Context*>);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, m_context);
    }
}

template <typename Context>
inline void CurlWriter<Context*>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

template <typename Context>
inline bool CurlWriter<Context*>::isValid() const
{
    return true;
}

}  // namespace network
