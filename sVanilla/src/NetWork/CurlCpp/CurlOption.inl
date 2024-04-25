#pragma once
#include <type_traits>

namespace network
{

template <typename COValueType>
inline CurlValueOption<COValueType>::~CurlValueOption()
{
    if constexpr (std::is_pointer_v<COValueType> && !std::is_const_v<std::remove_pointer_t<COValueType>>)
    {
        delete m_value;
        m_value = nullptr;
    }
}

template <typename COValueType>
inline void CurlValueOption<COValueType>::setValue(const ValueType& value)
{
    m_value = value;
}

template <typename COValueType>
inline const typename CurlValueOption<COValueType>::ValueType& CurlValueOption<COValueType>::getValue() const
{
    return m_value;
}

template <typename COValueType>
inline CurlValueOption<COValueType>* CurlValueOption<COValueType>::clone() const
{
    return new CurlValueOption<COValueType>(*this);
}

template <typename COValueType>
inline void CurlValueOption<COValueType>::updateOption(const AbstractOption& other)
{
    auto* pOther = dynamic_cast<const CurlValueOption<COValueType>*>(&other);
    if (pOther)
    {
        setValue(pOther->getValue());
    }
}

template <typename COValueType>
inline void CurlValueOption<COValueType>::setToCurl(CURL* handle) const
{
    if constexpr (std::is_same_v<std::string, COValueType>)
    {
        const CURLcode ret = curl_easy_setopt(handle, getOption(), m_value.c_str());
    }
    else
    {
        const CURLcode ret = curl_easy_setopt(handle, getOption(), m_value);
    }
}

template <typename COValueType>
inline CurlValueOption<COValueType>::CurlValueOption(ValueType value, CURLoption option)
    : AbstractOption(option)
    , m_value(std::move(value))
{
}

template <typename COValueType>
inline CurlValueOption<COValueType>::CurlValueOption(const CurlValueOption<COValueType>& other)
    : AbstractOption(other.getOption())
{
    if constexpr (std::is_pointer_v<COValueType> && !std::is_const_v<std::remove_pointer_t<COValueType>>)
    {
        using value_type = std::remove_pointer_t<COValueType>;
        m_value = new value_type(*other.m_value);
    }
    else
    {
        m_value = other.m_value;
    }
}

template <typename COValueType>
inline CurlValueOption<COValueType>::CurlValueOption(CURLoption option)
    : AbstractOption(option)
{
}

template <typename COValueType, CURLoption op>
inline CurlOption<COValueType, op>::CurlOption(typename CurlValueOption<COValueType>::ValueType value)
    : CurlValueOption<COValueType>(value, opt)
{
}

template <CURLoption op>
inline NoValueCurlOption<op>::NoValueCurlOption()
    : CurlOption<bool, op>(false)
{
}

template <CURLoption op>
inline NoValueCurlOption<op>* NoValueCurlOption<op>::clone() const
{
    return new NoValueCurlOption<op>();
}

}  // namespace network