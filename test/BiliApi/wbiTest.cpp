#include <gmock/gmock.h>
#include <Aria2Net/AriaClient/AriaClient.h>
#include "BiliApi/BilibiliUtils.h"

using namespace testing;
using namespace biliapi;

class BiliApiTest : public Test
{
public:
    CNetWork::ParamType params;

    std::vector<std::string> sortedParams;

    std::string query;

    std::string encoded;
    void SetUp() override
    {
        params["foo"] = "114";
        params["bar"] = "514";
        params["zab"] = "1919810";
        params["wts"] = "1702204169";

        for (auto& [key, value] : params)
        {
            // 过滤 value 中的 "!'()*" 字符
            const std::string filteredValue = filterCharacters(value);
            sortedParams.push_back(urlEncode(key) + "=" += urlEncode(filteredValue));
        }
        std::sort(sortedParams.begin(), sortedParams.end());

        query = std::accumulate(std::next(sortedParams.begin()), sortedParams.end(), sortedParams[0], [](std::string a, std::string b) {
            return std::move(a) + '&' + std::move(b);
        });
    }
};

TEST_F(BiliApiTest, URL)
{
    ASSERT_THAT(query, Eq("bar=514&foo=114&wts=1702204169&zab=1919810"));
}

TEST_F(BiliApiTest, URLEncode)
{
    encoded = biliapi::urlEncode(query);
    ASSERT_THAT(encoded, Eq("bar%3D514%26foo%3D114%26wts%3D1702204169%26zab%3D1919810"));
}

TEST_F(BiliApiTest, MixinKey)
{
    const std::string img_key = "7cd084941338484aae1ad9425b84077c";
    const std::string sub_key = "4932caff0ff746eab6f01bf08b70ac45";
    auto const mixin_key = biliapi::GetMixinKey(img_key + sub_key);
    ASSERT_THAT(mixin_key, Eq("ea1db124af3c7062474693fa704f4ff8"));
}

TEST_F(BiliApiTest, MD5)
{
    auto const origin = "bar=514&foo=114&wts=1702204169&zab=1919810ea1db124af3c7062474693fa704f4ff8";
    auto const w_rid = biliapi::MD5Hash(origin);
    ASSERT_THAT(w_rid, Eq("8f6f2b5b3d485fe1886cec6a0be8c5d4"));
}