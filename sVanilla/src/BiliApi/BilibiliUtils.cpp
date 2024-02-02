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
    unsigned char hash[MD5_DIGEST_LENGTH];

    MD5_CTX md5;
    MD5_Init(&md5);
    MD5_Update(&md5, str.c_str(), str.size());
    MD5_Final(hash, &md5);

    std::stringstream ss;

    for (const unsigned char i : hash){
        ss << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>( i );
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