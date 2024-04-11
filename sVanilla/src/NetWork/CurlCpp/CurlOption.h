#pragma once

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
    virtual void setToCurl(CurlEasy* handle) const = 0;

    virtual void clear() = 0;

protected:
    AbstractOption(CURLoption option);

private:
    CURLoption m_curlOption;
};

template<typename COValueType>
class CurlValueOption : public AbstractOption
{
public:
    using ValueType = COValueType;

    virtual ~CurlValueOption() = default;

    void setValue(const ValueType& value);
    const ValueType& getValue() const;

    CurlValueOption<COValueType>* clone() const override;
    void clear() override;
    void updateOption(const AbstractOption& other) override;

protected:
    CurlValueOption(ValueType value, CURLoption option);
    CurlValueOption(CURLoption option);

protected:
    ValueType m_value;
};

template<typename COValueType, CURLoption op>
class CurlOption : public CurlValueOption<COValueType>
{
public:
    static constexpr CurlOption opt = op;

};

template<CURLoption op>
class NoValueOptionTrait : public CurlOption<bool, op>
{
public:
    NoValueOptionTrait();
    NoValueOptionTrait * clone() const override;
};

// for curl
using Verbose = CurlOption<bool, CURLOPT_VERBOSE>;
using Header =CurlOption<bool, CURLOPT_HEADER>;
using NoProgress =CurlOption<bool, CURLOPT_NOPROGRESS>;

}  // namespace network

#include "CurlOption.inl"
