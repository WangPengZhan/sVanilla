#pragma once

// clang-format off
#define ISTR(x)                                                           #x
#define STR(x)                                                            ISTR(x)

#define ICAT(a, b)                                                        a##b
#define CAT(a, b)                                                         ICAT(a, b)

#define EXPAND(...)                                                       __VA_ARGS__

#define HELPER_OF_MACRO_COUNT(_1, _2, _3, _4, _5, _6, _7, _8, _9, N, ...) N
#define MACRO_COUNT(...)                                                  EXPAND(HELPER_OF_MACRO_COUNT(__VA_ARGS__, 9, 8, 7, 6, 5, 4, 3, 2, 1))
// clang-format on
