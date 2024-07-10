#include "BilibiliUrl.h"

#include <regex>

bool isValidUrl(const std::string& url)
{
    std::regex bilibiliPattern(R"((http|https):\/\/((www|m|space)\.)?(bilibili\.com|b23\.tv))");
    return std::regex_search(url, bilibiliPattern);
}

std::string getID(const std::string& url)
{
    if (!isValidUrl(url))
    {
        return {};
    }

    std::regex bvPattern(R"(BV([A-Za-z0-9]{10}))");
    std::regex avPattern(R"(av([0-9]+))");

    std::smatch match;
    if (std::regex_search(url, match, bvPattern))
    {
        return match[1].str();
    }
    if (std::regex_search(url, match, avPattern))
    {
        return match[1].str();
    }
    return "";
}