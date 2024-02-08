#include <openssl/md5.h>
#include <openssl/evp.h>
#include "BilibiliUtils.h"
#include "BiliApi.h"
#include <fstream>
#include <iomanip>
#include <iostream>
#include <numeric>
#include <sstream>
#include <curl/curl.h>

// size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *userp) {
//     userp->append((char*)contents, size * nmemb);
//     return size * nmemb;
// }

// 辅助函数，用于替换字符串中的所有目标子串为指定的新子串
void BiliApi::replaceCharacter(std::string& source, const std::string& from, const std::string& to)
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

void BiliApi::saveJson(const std::string& filename, const nlohmann::json& content)
{
    std::ofstream o(filename);
    if (!o)
    {
        throw std::runtime_error("Error opening file: " + filename);
    }
    o << content;
    o.close();
}
nlohmann::json BiliApi::readJson(const std::string& filename)
{
    std::ifstream file(filename);
    if (!file)
    {
        return nlohmann::json::object();
    }
    // 检查文件是否为空
    if (file.peek() == std::ifstream::traits_type::eof()) {
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
void BiliApi::updateData( const std::string& key, const nlohmann::json& value)
{
    nlohmann::json j = readJson("sVanilla.data");
    j[key] = value;
    saveJson("sVanilla.data", j);
}

std::string BiliApi::filterCharacters(const std::string& input)
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
std::string BiliApi::urlEncode(const std::string& decoded)
{
    const auto encoded_value = curl_easy_escape(nullptr, decoded.c_str(), static_cast<int>(decoded.length()));
    std::string result(encoded_value);
    curl_free(encoded_value);
    return result;
}
std::string BiliApi::urlDecode(const std::string& encoded)
{
    int output_length;
    const auto decoded_value = curl_easy_unescape(nullptr, encoded.c_str(), static_cast<int>(encoded.length()), &output_length);
    std::string result(decoded_value, output_length);
    curl_free(decoded_value);
    return result;
}

std::string BiliApi::GetMixinKey(const std::string& orig)
{
    std::string result;
    for (const int i : mixinKeyEncTab)
    {
        result += orig[i];
    }
    return result.substr(0, 32);
}
std::string BiliApi::MD5Hash(const std::string& str)
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
// void BiliApi::Get(){
//     CURL *curl;
//     CURLcode res;
//     std::string readBuffer;
//
//     curl_global_init(CURL_GLOBAL_DEFAULT);
//
//     curl = curl_easy_init();
//     if(curl) {
//         struct curl_slist *headers = nullptr;
//
//         headers = curl_slist_append(headers, "Referer: https://www.bilibili.com");
//         headers = curl_slist_append(headers, "User-Agent: Mozilla/5.0 (Macintosh; Intel Mac OS X 10_15_7) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/121.0.0.0 Safari/537.36");
//         headers = curl_slist_append(headers, "Cookie: SESSDATA=50cea82e%2C1722158630%2C52fbc%2A12CjCZ-m3IY6kROAiNMcziRaJ1r9W6od8FqErVzOW9aqqSZvhXs9_JeQRmG5Anz7uxO98SVkR3NDRWWDhOb21CeTRXY25BM28wdW15UHhpRElZdFZKQWNtNjRzbFNKRXUzUlM3OTgxTmVST0JjbnBMMDQzdnhab2RacjE1b1NvNjQwMW9uOXBHelh3IIEC; bili_jct=f25e423e574a3a36c74f1d2eaccaddfd; DedeUserID=1524815138; DedeUserID__ckMd5=f492745a735982a3; sid=on1qhrr5");
//
//         curl_easy_setopt(curl, CURLOPT_URL, "https://api.bilibili.com/x/player/wbi/playurl?bvid=BV1zv4y1T7NT&cid=761588849&fnval=0&fnver=0&fourk=1&qn=64&w_rid=59c826e1a65e12cdb4d56b30b06861ad&wts=1707370890");
//         curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
//
//         curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
//         curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
//
//         res = curl_easy_perform(curl);
//         if(res != CURLE_OK)
//             fprintf(stderr, "curl_easy_perform() failed: %s\n",
//                     curl_easy_strerror(res));
//         else
//             std::cout << readBuffer << std::endl;
//
//         curl_easy_cleanup(curl);
//
//         if(headers)
//             curl_slist_free_all(headers);
//     }
//
//     curl_global_cleanup();
// }

bool BiliApi::isExpired(const std::time_t& expires)
{
    return expires < std::time(nullptr) / 86400;
}