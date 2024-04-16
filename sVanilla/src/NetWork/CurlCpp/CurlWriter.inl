#pragma once
namespace network
{
template <typename Context, typename Writer>
inline CurlWriter<Context, Writer>::CurlWriter(Context& ctx)
    : m_context(ctx)
{
}

template <typename Context, typename Writer>
inline CurlWriter<Context, Writer>::CurlWriter(Context& ctx, Writer writer)
    : m_context(ctx)
    , m_writer(writer)
{
}

template <typename Context, typename Writer>
inline void CurlWriter<Context, Writer>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, m_writer);
        curl_easy_setopt(handle, CURLOPT_WRITEDATA, &m_context);
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

template <typename Response>
inline CurlResponseWrapper<Response>::CurlResponseWrapper(Response& response)
    : m_response(response)
    , m_writer(response)
{
}

template <typename Response>
inline void CurlResponseWrapper<Response>::setToCurl(CURL* handle)
{
    if (!handle)
    {
        return;
    }
    m_writer.setToCurl(handle);
}

template <typename Response>
inline void CurlResponseWrapper<Response>::setToCurl(CurlEasy* easy)
{
    if (easy)
    {
        setToCurl(easy->handle());
    }
}

template <typename Response>
inline void CurlResponseWrapper<Response>::readAfter(CURL* handle)
{
}

template <typename Response>
inline void CurlResponseWrapper<Response>::readAfter(CurlEasy* easy)
{
    if (easy)
    {
        readAfter(easy->handle());
    }
}

}  // namespace network
