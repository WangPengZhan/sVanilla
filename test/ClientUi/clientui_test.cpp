// ClientUi 模块单元测试。
//
// ClientUi 是主程序可执行入口，绝大部分代码是窗口/页面/平台初始化，无法作为库链接。
// 这里针对其中的**纯逻辑工具**做单元测试：
//   - UrlProcess.h  : removeSpecialChar()（文件名非法字符清理）
//   - SpeedUtil.h   : formatSize() / formatSpeed()（速度与容量格式化）
//   - SortItems.h   : sortInitialsItems()（按首字母排序）
//
// 这些头文件都是 header-only 且不依赖 QApplication，因此不需要事件循环。

#include <algorithm>
#include <functional>
#include <string>
#include <vector>

#include <QString>

#include <gtest/gtest.h>

#include "ClientUi/Utils/SortItems.h"
#include "ClientUi/Utils/SpeedUtil.h"
#include "ClientUi/Utils/UrlProcess.h"

// ---------- removeSpecialChar ----------

TEST(RemoveSpecialCharTest, StripsEveryCharacterInTheBlacklist)
{
    const std::string blacklist = util::specialChars;
    ASSERT_FALSE(blacklist.empty());

    const QString input = QString::fromStdString(blacklist);
    EXPECT_TRUE(util::removeSpecialChar(input).isEmpty());
}

TEST(RemoveSpecialCharTest, KeepsOrdinaryCharacters)
{
    EXPECT_EQ(util::removeSpecialChar(QString("normal-file_name 123.mp4")), QString("normal-file_name 123.mp4"));
}

TEST(RemoveSpecialCharTest, RemovesOnlyBlacklistedCharacters)
{
    // "a?b/c\"d<e>f*g|h:i" 去掉 ? / " < > * | : 后剩 "abcdefghi"
    EXPECT_EQ(util::removeSpecialChar(QString("a?b/c\"d<e>f*g|h:i")), QString("abcdefghi"));
}

TEST(RemoveSpecialCharTest, StdStringOverloadMatchesQStringOverload)
{
    const std::string input = "标题:第1集?.mp4";

    const std::string fromStdString = util::removeSpecialChar(input);
    const std::string fromQString = util::removeSpecialChar(QString::fromStdString(input)).toStdString();

    EXPECT_EQ(fromStdString, fromQString);
    EXPECT_EQ(fromStdString.find(':'), std::string::npos);
    EXPECT_EQ(fromStdString.find('?'), std::string::npos);
}

TEST(RemoveSpecialCharTest, EmptyInputStaysEmpty)
{
    EXPECT_TRUE(util::removeSpecialChar(QString()).isEmpty());
    EXPECT_TRUE(util::removeSpecialChar(std::string()).empty());
}

// ---------- formatSize / formatSpeed ----------

TEST(SpeedUtilTest, FormatsGigabytes)
{
    EXPECT_EQ(formatSize(1073741824LL), QString("1.00GiB"));
}

TEST(SpeedUtilTest, FormatsMebibytes)
{
    EXPECT_EQ(formatSize(1048576LL), QString("1.00Mib"));
    EXPECT_EQ(formatSize(2 * 1048576LL), QString("2.00Mib"));
}

// 回归：Kib 分支存在缺陷 —— 它把数值除以 Mib 并标成 "Mib"，
// 且因为上一个分支已经覆盖 >= Kib，最后那个 "Kib/s" 分支永远不可达。
TEST(SpeedUtilTest, KibibyteRangeIsFormattedAsMibByMistake)
{
    EXPECT_EQ(formatSize(1024LL), QString("0.00Mib"));  // 期望应为 1.00 KiB 一类，实际为 0.00Mib
    EXPECT_EQ(formatSize(2048LL), QString("0.00Mib"));
}

TEST(SpeedUtilTest, FormatsRawBytesBelowKibibyte)
{
    EXPECT_EQ(formatSize(512LL), QString("512.000B"));
    EXPECT_EQ(formatSize(0LL), QString("0.000B"));
}

TEST(SpeedUtilTest, FormatSpeedAppendsPerSecondSuffix)
{
    EXPECT_EQ(formatSpeed(1048576LL), QString("1.00Mib/s"));
    EXPECT_EQ(formatSpeed(512LL), QString("512.000B/s"));
}

// ---------- sortInitialsItems ----------
//
// 注意：提取函数的类型写作 std::function<QueryType(Sentence)>，QueryType/Sentence
// 都处在非推导上下文里，且不能从 lambda 推导 —— 必须传入真正的 std::function 对象，
// 否则编译报 "could not deduce template argument"。
// 该工具目前在主仓库中没有调用点。

namespace
{
std::function<QString(const QString&)> textExtractor()
{
    return [](const QString& value) {
        return value;
    };
}

std::function<int(const std::pair<int, QString>&)> keyExtractor()
{
    return [](const std::pair<int, QString>& item) {
        return item.first;
    };
}
}  // namespace

TEST(SortItemsTest, SortsAscending)
{
    std::vector<QString> items{"banana", "apple", "cherry"};
    sortInitialsItems(items, textExtractor(), Qt::AscendingOrder);

    const std::vector<QString> expected{"apple", "banana", "cherry"};
    EXPECT_EQ(items, expected);
}

TEST(SortItemsTest, SortsDescending)
{
    std::vector<QString> items{"banana", "apple", "cherry"};
    sortInitialsItems(items, textExtractor(), Qt::DescendingOrder);

    const std::vector<QString> expected{"cherry", "banana", "apple"};
    EXPECT_EQ(items, expected);
}

TEST(SortItemsTest, UsesPrebuiltTaskExtractor)
{
    std::vector<QString> items{"banana", "apple"};
    const std::function<QString(const QString&)> extractor = textExtractor();

    sortInitialsItems(items, extractor, Qt::AscendingOrder);

    EXPECT_EQ(items, (std::vector<QString>{"apple", "banana"}));
}

TEST(SortItemsTest, SortsByExtractedKeyNotByElement)
{
    std::vector<std::pair<int, QString>> items{
        {3, "c"},
        {1, "a"},
        {2, "b"}
    };
    sortInitialsItems(items, keyExtractor(), Qt::AscendingOrder);

    ASSERT_EQ(items.size(), 3u);
    EXPECT_EQ(items.at(0).first, 1);
    EXPECT_EQ(items.at(1).first, 2);
    EXPECT_EQ(items.at(2).first, 3);
}

TEST(SortItemsTest, EmptyCollectionIsSafe)
{
    std::vector<QString> empty;
    sortInitialsItems(empty, textExtractor(), Qt::AscendingOrder);
    EXPECT_TRUE(empty.empty());
}

TEST(SortItemsTest, SingleElementIsSafe)
{
    std::vector<QString> single{"only"};
    sortInitialsItems(single, textExtractor(), Qt::DescendingOrder);
    EXPECT_EQ(single.at(0), QString("only"));
}
