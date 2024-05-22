#pragma once
#include <string>
#include <nlohmann/json.hpp>

#include "BiliApi.h"

namespace biliapi
{

inline const std::vector mixinKeyEncTab = {46, 47, 18, 2,  53, 8,  23, 32, 15, 50, 10, 31, 58, 3,  45, 35, 27, 43, 5,  49, 33, 9,
                                     42, 19, 29, 28, 14, 39, 12, 38, 41, 13, 37, 48, 7,  16, 24, 55, 40, 61, 26, 17, 0,  1,
                                     60, 51, 30, 4,  22, 25, 54, 21, 56, 59, 6,  63, 57, 62, 11, 36, 20, 34, 44, 52};
void replaceCharacter(std::string& source, const std::string& from, const std::string& to);
void saveJson(const std::string& filename, const nlohmann::json& content);
nlohmann::json readJson(const std::string& filename);
void updateData(const std::string& key, const nlohmann::json& value);
std::string filterCharacters(const std::string& input);
std::string urlEncode(const std::string& decoded);
std::string urlDecode(const std::string& encoded);
std::string GetMixinKey(const std::string& orig);
std::string MD5Hash(const std::string& str);
bool isExpired(const std::time_t& expires);

template <typename Result>
bool readData(Result& result, const std::string& key)
{
    nlohmann::json j = readJson("sVanilla.data");
    if (!j.is_object() || !j.contains(key))
    {
        return false;
    }

    nlohmann::json cookie = j[key];
    if (!cookie.is_object())
    {
        return false;
    }

    if (cookie.contains("Expires") && isExpired(cookie["Expires"].get<std::time_t>()))
    {
        return false;
    }

    result = Result(cookie);
    return true;
}
}  // namespace biliapi