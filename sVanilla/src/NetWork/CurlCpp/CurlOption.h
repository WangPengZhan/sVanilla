#pragma once
#include <string>

#include <curl/curl.h>

namespace network
{

class CurlEasy;

class AbstractOption
{
public:
    virtual ~AbstractOption() = default;

    CURLoption getOption() const;

    virtual AbstractOption* clone() const = 0;
    virtual void updateOption(const AbstractOption& other) = 0;
    virtual void setToCurl(CURL* handle) const = 0;

protected:
    AbstractOption(CURLoption option);

private:
    CURLoption m_curlOption;
};

template <typename COValueType>
class CurlValueOption : public AbstractOption
{
public:
    using ValueType = COValueType;

    ~CurlValueOption();

    void setValue(const ValueType& value);
    const ValueType& getValue() const;

    CurlValueOption<COValueType>* clone() const override;
    void updateOption(const AbstractOption& other) override;
    void setToCurl(CURL* handle) const override;

protected:
    CurlValueOption(ValueType value, CURLoption option);
    CurlValueOption(const CurlValueOption<COValueType>& other);
    CurlValueOption(CURLoption option);

protected:
    ValueType m_value;
};

template <typename COValueType, CURLoption op>
class CurlOption : public CurlValueOption<COValueType>
{
public:
    static constexpr CURLoption opt = op;
    CurlOption(typename CurlValueOption<COValueType>::ValueType value);
};

template <CURLoption op>
class NoValueCurlOption : public CurlOption<bool, op>
{
public:
    NoValueCurlOption();
    NoValueCurlOption<op>* clone() const override;
};

// for curl option type
class CurlCookies;
using Verbose = CurlOption<bool, CURLOPT_VERBOSE>;
using Header = CurlOption<bool, CURLOPT_HEADER>;
using NoProgress = CurlOption<bool, CURLOPT_NOPROGRESS>;
using TimeOut = CurlOption<long, CURLOPT_TIMEOUT>;
using AcceptEncoding = CurlOption<const char*, CURLOPT_ACCEPT_ENCODING>;
using SSLVerifyPeer = CurlOption<bool, CURLOPT_SSL_VERIFYPEER>;
using SSLVerifyHost = CurlOption<bool, CURLOPT_SSL_VERIFYHOST>;
using PostFields = CurlOption<std::string, CURLOPT_POSTFIELDS>;
using CookieFileds = CurlOption<CurlCookies, CURLOPT_COOKIE>;

}  // namespace network

#include "CurlOption.inl"
