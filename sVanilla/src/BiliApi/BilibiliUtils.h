#pragma once
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

template <typename T>
bool readData(const std::string& filename, const std::string& key, T& value)
{
    nlohmann::json j = readJson(filename);
    auto it = T(j.find(key));
    const time_t expires = it.find("Expires");
    if (const std::time_t now = std::time(nullptr) / 86400; expires < now)
    {
        return false;
    }
    value = it;
    return true;
}
}  // namespace BiliApi