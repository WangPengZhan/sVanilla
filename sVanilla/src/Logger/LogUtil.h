#pragma once
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <string>

#include "Base/MacroTool.h"

#define LogT(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::trace, __VA_ARGS__)
#define LogD(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::debug, __VA_ARGS__)
#define LogI(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::info, __VA_ARGS__)
#define LogW(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::warn, __VA_ARGS__)
#define LogE(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::err, __VA_ARGS__)
#define LogC(...) SPDLOG_LOGGER_CALL(spdlog::get(Logger::defaultName), spdlog::level::critical, __VA_ARGS__)

template <typename ReleaseFunc>
class scope_guard
{
public:
    scope_guard(ReleaseFunc func)
        : m_func(std::move(func))
    {
    }

    ~scope_guard()
    {
        if (!isReleased)
        {
            m_func();
            isReleased = true;
        }
    }

private:
    ReleaseFunc m_func;
    bool isReleased = false;
};

template <typename Lambda>
scope_guard(Lambda&&) -> scope_guard<std::remove_reference_t<Lambda>>;

// clang-format off
#define LogLimitTime_2(LOG, Time)                                                                                 \
    scope_guard CAT(scope_, __COUNTER__) = [func_name = __FUNCTION__,                                             \
                                                      start = std::chrono::high_resolution_clock::now(),          \
                                                      limit_duration = std::chrono::nanoseconds{Time}]() {        \
        auto duration = std::chrono::high_resolution_clock::now() - start;                                        \
        if (duration >= limit_duration) {                                                                         \
            LOG("exit function: {}, cost: {} ms, expect in {} ms", func_name,                                     \
                 std::chrono::duration_cast<std::chrono::milliseconds>(duration).count(),                         \
                 std::chrono::duration_cast<std::chrono::milliseconds>(limit_duration).count());                  \
        }                                                                                                         \
    }

#define LogLimitTime_1(LOG) LogLimitTime_2(LOG, 100000000)
#define LogLimitTime(...) EXPAND(CAT(LogLimitTime_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define LogLimitTimeI() LogLimitTime(LogI)

#define LogTimer_1(LOG)                                                                                             \
    scope_guard CAT(scope_, __COUNTER__) = [func_name = __FUNCTION__,                                             \
                                                      start = std::chrono::high_resolution_clock::now()]() {      \
        auto duration = std::chrono::high_resolution_clock::now() - start;                                        \
        LOG("exit function: {}, cost: {} ms", func_name,                                                          \
             std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());                            \
    }
#define LogTimer(...) EXPAND(CAT(LogLimitTime_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define LogTimerI() LogTimer(LogI)

#define Log_Unique_Timer_2(KEY, LOG)                                                                              \
    scope_guard  CAT(KEY, _Unique_Log) = [func_name = __FUNCTION__,                                               \
                                                      start = std::chrono::high_resolution_clock::now()]() {      \
        auto duration = std::chrono::high_resolution_clock::now() - start;                                        \
        LOG("exit function: {}, cost: {} ms", func_name,                                                          \
             std::chrono::duration_cast<std::chrono::milliseconds>(duration).count());                            \
    }
#define Log_Unique_Timer_1(KEY) Log_Unique_Timer_2(KEY, LogI)
#define Log_Unique_Timer(...) EXPAND(CAT(Log_Unique_Timer_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define Log_Unique_TimerI() Log_Unique_Timer(KEY_Start))
#define Log_Unique_Timer_END(KEY) CAT(KEY, _Unique_Log).~scope_guard();
#define Log_Unique_TimerI_END() Log_Unique_Timer_END(KEY_Start)
                                                                      
#define LogFuncT_1(LOG)                                                                                           \
    LOG("enter function: {}", __FUNCTION__);                                                                      \
    LogTimer(LOG)
#define LogFuncT(...) EXPAND(CAT(LogFuncT_, MACRO_COUNT(__VA_ARGS__))(__VA_ARGS__))
#define LogFuncTI() LogFuncT(LogI)

// clang-format on
