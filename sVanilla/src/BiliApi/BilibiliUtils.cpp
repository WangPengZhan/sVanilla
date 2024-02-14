#include <openssl/md5.h>
#include <openssl/evp.h>

#include <iomanip>
#include <numeric>
#include <sstream>
#include <algorithm>

#include <curl/curl.h>

#include "BilibiliUtils.h"

namespace BiliApi
{
// 辅助函数，用于替换字符串中的所有目标子串为指定的新子串
void replaceAll(std::string& source, const std::string& from, const std::string& to) 
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
}// 过滤字符串中的特定字符


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
std::string BiliApi::url_encode(const std::string& decoded)
{
    const auto encoded_value = curl_easy_escape(nullptr, decoded.c_str(), static_cast<int>(decoded.length()));
    std::string result(encoded_value);
    curl_free(encoded_value);
    return result;
}
std::string BiliApi::url_decode(const std::string& encoded){
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
void BiliApi::encWbi(CNetWork::ParamType& params, const std::string& mixin_key)
{
    // 添加 wts 字段
    const time_t curr_time = time(nullptr);
    params["wts"] = std::to_string(curr_time);

    // 按照 key 重排参数
    std::vector<std::string> sortedParams;
    for (auto& [key, value] : params)
    {
        // 过滤 value 中的 "!'()*" 字符
        const std::string filteredValue = filterCharacters(value);
        // url encode
        sortedParams.push_back(url_encode(key) + "=" += url_encode(filteredValue));
    }
    std::sort(sortedParams.begin(), sortedParams.end());
    // 序列化参数
    const std::string query = std::accumulate(std::next(sortedParams.begin()), sortedParams.end(), sortedParams[0], [](std::string a, std::string b) {
        return std::move(a) + '&' + std::move(b);
    });

    // 计算 w_rid 字段
    params["w_rid"] = MD5Hash(query + mixin_key);
}

} // BiliApi
