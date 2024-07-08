#include <openssl/md5.h>
#include <openssl/evp.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <algorithm>
#include <curl/curl.h>

#include "BiliApi.h"
#include "BilibiliUtils.h"

namespace biliapi
{
std::string cookieDataDir;

// 辅助函数，用于替换字符串中的所有目标子串为指定的新子串
void replaceCharacter(std::string& source, const std::string& from, const std::string& to)
{
    std::string newString;
    newString.reserve(source.length());  // 预分配足够空间

    std::string::size_type lastPos = 0;
    std::string::size_type findPos;

    while (std::string::npos != (findPos = source.find(from, lastPos)))
    {
        newString.append(source, lastPos, findPos - lastPos);
        newString += to;
        lastPos = findPos + from.length();
    }

    // 拼接最后一个分隔符后的所有字符
    newString += source.substr(lastPos);
    source.swap(newString);
}

void saveJson(const std::string& filename, const nlohmann::json& content)
{
    std::ofstream o(filename);
    if (!o)
    {
        throw std::runtime_error("Error opening file: " + filename);
    }
    o << content;
    o.close();
}

nlohmann::json readJson(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        return nlohmann::json::object();
    }
    // 检查文件是否为空
    if (file.peek() == std::ifstream::traits_type::eof())
    {
        // 文件为空，返回空的json对象
        return nlohmann::json::object();
    }

    nlohmann::json j;
    try
    {
        file >> j;
    }
    catch (nlohmann::json::parse_error& e)
    {
        throw std::runtime_error("Error parsing JSON: " + std::string(e.what()));
    }
    return j;
}

void updateData(const std::string& key, const nlohmann::json& value)
{
    const std::string filename = cookieDataFilePath();
    nlohmann::json j = readJson(filename);
    j[key] = value;
    saveJson(filename, j);
}

std::string filterCharacters(const std::string& input)
{
    const std::string charsToFilter = "!\'()*";
    std::string result = input;

    // 使用remove_if和lambda表达式来移除需要过滤的字符
    result.erase(std::remove_if(result.begin(), result.end(),
                                [&charsToFilter](const char& c) {
                                    return charsToFilter.find(c) != std::string::npos;
                                }),
                 result.end());

    return result;
}

std::string urlEncode(const std::string& decoded)
{
    auto* const encoded_value = curl_easy_escape(nullptr, decoded.c_str(), static_cast<int>(decoded.length()));
    std::string result(encoded_value);
    curl_free(encoded_value);
    return result;
}

std::string urlDecode(const std::string& encoded)
{
    int output_length;
    auto* const decoded_value = curl_easy_unescape(nullptr, encoded.c_str(), static_cast<int>(encoded.length()), &output_length);
    std::string result(decoded_value, output_length);
    curl_free(decoded_value);
    return result;
}

std::string GetMixinKey(const std::string& orig)
{
    std::string result;
    for (const int i : mixinKeyEncTab)
    {
        result += orig[i];
    }
    return result.substr(0, 32);
}

std::string MD5Hash(const std::string& str)
{
    unsigned char md_value[EVP_MAX_MD_SIZE];
    unsigned int md_len;
    // 创建和初始化摘要上下文
    EVP_MD_CTX* mdctx = EVP_MD_CTX_new();
    const EVP_MD* md = EVP_md5();
    // 初始化摘要操作
    EVP_DigestInit_ex(mdctx, md, nullptr);
    // 提供要进行摘要计算的数据
    EVP_DigestUpdate(mdctx, str.c_str(), str.length());
    // 获取摘要结果
    EVP_DigestFinal_ex(mdctx, md_value, &md_len);
    // 清理上下文
    EVP_MD_CTX_free(mdctx);
    // 将摘要结果转换为字符串形式
    std::ostringstream oss;
    for (unsigned int i = 0; i < md_len; i++)
    {
        oss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(md_value[i]);
    }

    return oss.str();
}

bool isExpired(const std::time_t& expires)
{
    return expires < std::time(nullptr) / 86400;
}

void setCookieDataDir(const std::string& dir)
{
    cookieDataDir = dir;
}

std::string cookieDataFilePath()
{
    std::string filePath;
    if (cookieDataDir.empty())
    {
        filePath = cookieDataFileName;
    }
    else
    {
        filePath =
            (cookieDataDir.back() == '/' || cookieDataDir.back() == '\\') ? cookieDataDir + cookieDataFileName : cookieDataDir + "/" + cookieDataFileName;
    }

    return filePath;
}

}  // namespace biliapi
