#pragma once
namespace network
{
template <typename Context, typename Writer>
inline CurlWriter<Context, Writer>::CurlWriter(Context* ctx)
    : m_pContext(ctx)
{
}

template <typename Context, typename Writer>
inline CurlWriter<Context, Writer>::CurlWriter(Context* ctx, Writer writer)
    : m_pContext(ctx)
    , m_writer(writer)
{
}

template <typename Context, typename Writer>
inline void CurlWriter<Context, Writer>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, m_writer);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, m_pContext);
    }
}

template <typename Context, typename Writer>
inline void CurlWriter<Context, Writer>::setToCurl(CurlEasy* easy)
{
    if (easy)
    {
        setToCurl(easy->handle());
    }
}

template <typename Context, typename Writer>
inline void CurlWriter<Context, Writer>::isValid() const
{
    return true;
}
}  // namespace network
