#include <iomanip>
#include <sstream>
#include <chrono>
#include "TimerUtil.h"

std::string convertTimestamp(long long timestamp)
{
    std::chrono::time_point<std::chrono::system_clock> tp = std::chrono::system_clock::from_time_t(static_cast<std::time_t>(timestamp));
    std::time_t tt = std::chrono::system_clock::to_time_t(tp);
    std::tm* t = std::gmtime(&tt);
    std::ostringstream oss;
    oss << std::put_time(t, "%F");
    return oss.str();
}

std::string formatDuration(const int duration)
{
    std::chrono::seconds sec(duration);
    const auto hours = std::chrono::duration_cast<std::chrono::hours>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(hours);
    const auto minutes = std::chrono::duration_cast<std::chrono::minutes>(sec);
    sec -= std::chrono::duration_cast<std::chrono::seconds>(minutes);

    std::ostringstream formattedDuration;
    if (hours.count() > 0)
    {
        formattedDuration << std::setfill('0') << std::setw(2) << hours.count() << ":";
    }
    formattedDuration << std::setfill('0') << std::setw(2) << minutes.count() << ":" << std::setfill('0') << std::setw(2) << sec.count();

    return formattedDuration.str();
}