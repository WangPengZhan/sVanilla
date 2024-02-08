#pragma once
#include "BiliApi.h"

#include <string>
#include "NetWork/CNetWork.h"
#include <nlohmann/json.hpp>

namespace BiliApi
{

inline std::vector mixinKeyEncTab = {46, 47, 18, 2,  53, 8,  23, 32, 15, 50, 10, 31, 58, 3,  45, 35, 27, 43, 5,  49, 33, 9,
                                     42, 19, 29, 28, 14, 39, 12, 38, 41, 13, 37, 48, 7,  16, 24, 55, 40, 61, 26, 17, 0,  1,
                                     60, 51, 30, 4,  22, 25, 54, 21, 56, 59, 6,  63, 57, 62, 11, 36, 20, 34, 44, 52};
void replaceCharacter(std::string& source, const std::string& from, const std::string& to);
void saveJson(const std::string& filename, const nlohmann::json& content);
nlohmann::json readJson(const std::string& filename);
void updateData(const std::string& filename, const std::string& key, const nlohmann::json& value);
std::string filterCharacters(const std::string& input);
std::string url_encode(const std::string& decoded);
std::string url_decode(const std::string& encoded);
std::string GetMixinKey(const std::string& orig);
std::string MD5Hash(const std::string& str);

bool isExpired(const std::time_t& expires);

template <typename Result>
Result readData(const std::string& key)
{
    nlohmann::json j = readJson("sVanilla.data");
    if (!j.is_object() || !j.contains(key))
    {
        return Result();
    }

    nlohmann::json cookie = j[key];
    if (!cookie.is_object())
    {
        return Result();
    }

    if (cookie.contains("Expires") && isExpired(cookie["Expires"].get<std::time_t>()))
    {
        return Result();
    }

    return Result(cookie);
}
Cookie readCookie();
MixinKey readMixinKey();

}  // namespace BiliApi