#include <curl/curl.h>

#include "CurlGlobal.h"

namespace network
{
CurlGlobal::CurlGlobal()
{
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlGlobal::CurlGlobal(long flag)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

CurlGlobal::~CurlGlobal()
{
    curl_global_cleanup();
}
} // namespace network
