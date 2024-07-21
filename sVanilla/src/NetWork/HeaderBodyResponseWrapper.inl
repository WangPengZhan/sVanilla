#pragma once

namespace network
{
template <typename Context>
inline CurlHeaderWriter<Context>::CurlHeaderWriter(Context& ctx)
    : m_context(ctx)
{
}

template <typename Context>
inline void CurlHeaderWriter<Context>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, writeFunc<Context>);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, &m_context);
    }
}

template <typename Context>
inline void CurlHeaderWriter<Context>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

template <typename Context>
inline bool CurlHeaderWriter<Context>::isValid() const
{
    return true;
}

template <typename Context>
inline CurlHeaderWriter<Context*>::CurlHeaderWriter(Context* ctx)
    : m_context(ctx)
{
}

template <typename Context>
inline void CurlHeaderWriter<Context*>::setToCurl(CURL* handle)
{
    if (handle)
    {
        curl_easy_setopt(handle, CURLOPT_HEADERFUNCTION, writeFunc<Context*>);
        curl_easy_setopt(handle, CURLOPT_HEADERDATA, m_context);
    }
}

template <typename Context>
inline void CurlHeaderWriter<Context*>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

template <typename Context>
inline bool CurlHeaderWriter<Context*>::isValid() const
{
    return true;
}

inline CurlResponseWrapper<ResponseHeaderAndBody>::CurlResponseWrapper(ResponseHeaderAndBody& response)
    : m_response(response)
    , m_writer(m_response.body)
    , m_headerWriter(m_response.header)
{
}

inline void CurlResponseWrapper<ResponseHeaderAndBody>::setToCurl(CURL* handle)
{
    if (handle)
    {
        m_writer.setToCurl(handle);
        m_headerWriter.setToCurl(handle);
    }
}

inline void CurlResponseWrapper<ResponseHeaderAndBody>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

}  // namespace network
