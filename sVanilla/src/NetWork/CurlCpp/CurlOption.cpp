#include "CurlOption.h"

namespace network
{
CURLoption AbstractOption::getOption() const
{
    return m_curlOption;
}

AbstractOption::AbstractOption(CURLoption option)
    : m_curlOption(option)
{
}

}  // namespace network
