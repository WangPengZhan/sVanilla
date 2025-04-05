#include <vector>
#include <chrono>
#include <thread>
#include <map>
#include <any>

#include <gtest/gtest.h>

#include "Zipper/Zipper.h"

TEST(Zipper_Test, zip)
{
    Zipper zipper;
    zipper.setZipPath("D:/workfile/sVanilla/test/TestData/Zipper/zipper");
    zipper.setOutputFileName("D:/workfile/sVanilla/test/TestData/Zipper/zipped.zip");
    bool result = zipper.zip();
    EXPECT_EQ(result, true);
}

TEST(Zipper_Test, unzip)
{
    Unzipper unzipper;
    unzipper.setUnzippedFile("D:/workfile/sVanilla/test/TestData/Zipper/unzipper/zipper.zip");
    unzipper.setOutputPath("D:/workfile/sVanilla/test/TestData/Zipper/unzipper/");
    bool result = unzipper.unzip();
    EXPECT_EQ(result, true);
}

#include <iostream>
#include <string>
#include <unordered_set>
#include <string_view>

static constexpr char32_t specialChars[] = U"?/\"\\<>*|:•";

std::string removeSpecialUtf8Chars(const std::string& input)
{
    std::unordered_set<char32_t> specialSet(std::begin(specialChars), std::end(specialChars));

    std::string result;
    std::u8string_view sv(reinterpret_cast<const char8_t*>(input.data()), input.size());

    for (size_t i = 0; i < sv.size();)
    {
        char32_t cp = 0;
        unsigned char lead = sv[i];

        size_t length = 0;
        if (lead < 0x80)
        {
            cp = lead;
            length = 1;
        }
        else if ((lead >> 5) == 0x6)
        {
            cp = (lead & 0x1F) << 6 | (sv[i + 1] & 0x3F);
            length = 2;
        }
        else if ((lead >> 4) == 0xE)
        {
            cp = (lead & 0x0F) << 12 | (sv[i + 1] & 0x3F) << 6 | (sv[i + 2] & 0x3F);
            length = 3;
        }
        else if ((lead >> 3) == 0x1E)
        {
            cp = (lead & 0x07) << 18 | (sv[i + 1] & 0x3F) << 12 | (sv[i + 2] & 0x3F) << 6 | (sv[i + 3] & 0x3F);
            length = 4;
        }
        else
        {
            ++i;
            continue;
        }

        if (specialSet.find(cp) == specialSet.end())
        {
            result.append(reinterpret_cast<const char*>(&sv[i]), length);
        }

        i += length;
    }

    return result;
}

TEST(Zipper_Test, utf_8_test)
{
    std::string input = "Hello?世界/你好•|";
    std::string output = removeSpecialUtf8Chars(input);
    std::cout << "原始: " << input << "\n";
    std::cout << "结果: " << output << "\n";
}
