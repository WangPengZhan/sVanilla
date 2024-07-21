#pragma once
#include <string>

#include "CurlEasy.h"

namespace network
{
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

#include "CurlResponseWrapper.inl"
