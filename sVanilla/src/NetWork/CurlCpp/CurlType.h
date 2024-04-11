#pragma once
#include <functional>

#include <curl/curl.h>

namespace network
{
    using WriteFunc = size_t(*)(char*, size_t, size_t);
    using ReadFunc = size_t(*)(char*, size_t, size_t);
    using DebugFunc = int(*)(curl_infotype*, char*, size_t);
    using SslCtxFunc = CURLcode(*)(void *);
    using ProgressFunc = int(*)(double, double, double, double);

	using WriteFunctionFunctor =  std::function<WriteFunc>;
	using ReadFunctionFunctor = std::function<ReadFunc>;
    using DebugFunctionFunctor = std::function<DebugFunc>;
    using SslCtxFunctionFunctor = std::function<SslCtxFunc>;
	using ProgressFunctionFunctor = std::function<ProgressFunc>;

} // namespace network