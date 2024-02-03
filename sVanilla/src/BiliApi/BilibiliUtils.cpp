#include <openssl/md5.h>
#include <openssl/evp.h>
#include "BilibiliUtils.h"

#include <iomanip>
#include <numeric>
#include <sstream>

// 过滤字符串中的特定字符
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
    // unsigned char hash[MD5_DIGEST_LENGTH];
    //
    // MD5_CTX md5;
    // MD5_Init(&md5);
    // MD5_Update(&md5, str.c_str(), str.size());
    // MD5_Final(hash, &md5);
    //
    // std::stringstream ss;
    //
    // for (const unsigned char i : hash){
    //     ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( i );
    // }
    // return ss.str();
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
        std::string filteredValue = filterCharacters(value);
        sortedParams.push_back(key + "=" + filteredValue);
    }
    std::sort(sortedParams.begin(), sortedParams.end());
    // 序列化参数
    std::string query = std::accumulate(std::next(sortedParams.begin()), sortedParams.end(), sortedParams[0], [](std::string a, std::string b) {
        return std::move(a) + '&' + std::move(b);
    });

    // 计算 w_rid 字段
    params["w_rid"] = MD5Hash(query + mixin_key);
    // 最后原始wts 字段到请求参数
    params["wts"] = std::to_string(curr_time);
}