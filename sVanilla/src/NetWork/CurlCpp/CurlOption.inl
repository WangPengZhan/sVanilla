

namespace network
{

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
    return CurlValueOption<COValueType>();
}

template <typename COValueType>
inline void CurlValueOption<COValueType>::clear()
{
}

template <typename COValueType>
inline void CurlValueOption<COValueType>::updateOption(const AbstractOption& other)
{
}

template <typename COValueType>
inline CurlValueOption<COValueType>::CurlValueOption(ValueType value, CURLoption option)
{
}

template <typename COValueType>
inline CurlValueOption<COValueType>::CurlValueOption(CURLoption option)
{
}

} // namespace network