#pragma once

// The way how the function is called
#if !defined(LOGGER_CALL)
#    if defined(_WIN32)
#        define LOGGER_CALL __stdcall
#    else
#        define LOGGER_CALL
#    endif /* _WIN32 */
#endif     /* LOGGER_CALL */

// The function exported symbols
#if defined _WIN32 || defined __CYGWIN__
#    define LOGGER_DLL_IMPORT __declspec(dllimport)
#    define LOGGER_DLL_EXPORT __declspec(dllexport)
#    define LOGGER_DLL_LOCAL
#else
#    if __GNUC__ >= 4
#        define LOGGER_DLL_IMPORT __attribute__((visibility("default")))
#        define LOGGER_DLL_EXPORT __attribute__((visibility("default")))
#        define LOGGER_DLL_LOCAL  __attribute__((visibility("hidden")))
#    else
#        define LOGGER_DLL_IMPORT
#        define LOGGER_DLL_EXPORT
#        define LOGGER_DLL_LOCAL
#    endif
#endif

#ifdef LOGGER_LOCAL
#    define LOGGER_PORT
#    define LOGGER_API LOGGER_DLL_LOCAL
#else
#    ifdef LOGGER_DLL_EXPORTS  // defined if we are building the DLL (instead of using it)
#        define LOGGER_PORT LOGGER_DLL_EXPORT
#        define LOGGER_API  LOGGER_DLL_EXPORT LOGGER_CALL
#    else
#        define LOGGER_PORT LOGGER_DLL_IMPORT
#        define LOGGER_API  LOGGER_DLL_IMPORT LOGGER_CALL
#    endif  // LOGGER_DLL_EXPORTS
#endif