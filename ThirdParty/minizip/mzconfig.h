#pragma once

// The way how the function is called
#if !defined(MZ_CALL)
#if defined(_WIN32)
#define MZ_CALL __stdcall
#else
#define MZ_CALL
#endif /* _WIN32 */
#endif /* MZ_CALL */

// The function exported symbols
#if defined _WIN32 || defined __CYGWIN__
#define MZ_DLL_IMPORT __declspec(dllimport)
#define MZ_DLL_EXPORT __declspec(dllexport)
#define MZ_DLL_LOCAL
#else
#if __GNUC__ >= 4
#define MZ_DLL_IMPORT __attribute__((visibility("default")))
#define MZ_DLL_EXPORT __attribute__((visibility("default")))
#define MZ_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define MZ_DLL_IMPORT
#define MZ_DLL_EXPORT
#define MZ_DLL_LOCAL
#endif
#endif

#ifdef MZ_LOCAL
#define MZEXPORT MZ_DLL_LOCAL
#else
#ifdef MZ_DLL_EXPORTS // defined if we are building the DLL (instead of using it)
#define MZEXPORT MZ_DLL_EXPORT MZ_CALL
#else
#define MZEXPORT MZ_DLL_IMPORT MZ_CALL
#endif // MZ_DLL_EXPORTS
#endif