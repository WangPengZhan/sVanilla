#include <openssl/md5.h>
#include <openssl/evp.h>
#include "BilibiliUtils.h"

#include <iomanip>
#include <sstream>

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
    std::vector<unsigned char> md5_digest(MD5_DIGEST_LENGTH);
    EVP_MD_CTX* md5_ctx = EVP_MD_CTX_new();
    EVP_DigestInit_ex(md5_ctx, EVP_md5(), nullptr);
    EVP_DigestUpdate(md5_ctx, str.c_str(), str.size());
    EVP_DigestFinal_ex(md5_ctx, md5_digest.data(), nullptr);
    EVP_MD_CTX_free(md5_ctx);

    std::stringstream ss;
    ss << std::hex << std::setfill('0');
    for (unsigned char c : md5_digest)
    {
        ss << std::setw(2) << static_cast<int>(c);
    }
    return ss.str();
}
void BiliApi::encWbi(CNetWork::ParamType& params, const std::string& mixin_key)
{
    // 添加 wts 字段
    const time_t curr_time = time(nullptr);
    params["wts"] = std::to_string(curr_time);
    //过滤 value 中的 "!'()*" 字符
    for (auto& [fst, snd] : params)
    {
        std::string value = snd;
        value.erase(std::remove_if(value.begin(), value.end(), [](char c) {
            return c == '!' || c == '\'' || c == '(' || c == ')' || c == '*';
        }), value.end());
        snd = value;
    }

    std::string query;
    for (const auto& [fst, snd] : params)
    {
        if (!query.empty())
        {
            query += "&";
        }
        query.append(fst).append("=").append(snd);
    }

    params["w_rid"] = MD5Hash(query + mixin_key);
}