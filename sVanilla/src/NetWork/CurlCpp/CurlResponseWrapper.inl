#pragma once
#include <functional>

namespace network
{

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
inline void CurlResponseWrapper<Response>::setToCurl(CurlEasy& easy)
{
    setToCurl(easy.handle());
}

template <typename Response>
inline void CurlResponseWrapper<Response>::readAfter(CURL* handle)
{
}

template <typename Response>
inline void CurlResponseWrapper<Response>::readAfter(CurlEasy& easy)
{
    readAfter(easy.handle());
}

}  // namespace network
