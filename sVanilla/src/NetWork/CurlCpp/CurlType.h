#pragma once
#include <functional>
#include <type_traits>

#include <curl/curl.h>

namespace network
{
using WriteFunc = size_t (*)(void*, size_t, size_t, void*);
using ReadFunc = size_t (*)(void*, size_t, size_t, void*);
using DebugFunc = int (*)(CURL*, curl_infotype, char*, size_t, void*);
using SslCtxFunc = CURLcode (*)(CURL*, void*, void*);
using ProgressFunc = int (*)(void*, double, double, double, double);

using WriteFunctionFunctor = std::function<std::remove_pointer_t<WriteFunc>>;
using ReadFunctionFunctor = std::function<std::remove_pointer_t<ReadFunc>>;
using DebugFunctionFunctor = std::function<std::remove_pointer_t<DebugFunc>>;
using SslCtxFunctionFunctor = std::function<std::remove_pointer_t<SslCtxFunc>>;
using ProgressFunctionFunctor = std::function<std::remove_pointer_t<ProgressFunc>>;

}  // namespace network