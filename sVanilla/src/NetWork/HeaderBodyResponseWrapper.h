#pragma once
#include <string>

#include "CurlCpp/CurlResponseWrapper.h"
#include "CurlCpp/CurlWriter.h"

namespace network
{

template <typename Context>
class CurlHeaderWriter
{
public:
    CurlHeaderWriter(Context& ctx);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    bool isValid() const;

private:
    Context& m_context;
};

template <typename Context>
class CurlHeaderWriter<Context*>
{
public:
    CurlHeaderWriter(Context* ctx);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    bool isValid() const;

private:
    Context* m_context;
};

struct ResponseHeaderAndBody
{
    std::string body;
    std::string header;
};

template <>
class CurlResponseWrapper<ResponseHeaderAndBody>
{
public:
    CurlResponseWrapper(ResponseHeaderAndBody& response);

    void setToCurl(CURL* handle);
    void setToCurl(CurlEasy& easy);

    void readAfter(CURL* handle);
    void readAfter(CurlEasy& easy);

private:
    ResponseHeaderAndBody& m_response;
    CurlWriter<decltype(std::declval<ResponseHeaderAndBody>().body)> m_writer;
    CurlHeaderWriter<decltype(std::declval<ResponseHeaderAndBody>().header)> m_headerWriter;
};

}  // namespace network

#include "HeaderBodyResponseWrapper.inl"
