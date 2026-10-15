// Base 模块单元测试。
//
// Base 是纯头文件模块，只有一个 MacroTool.h，提供 STR / CAT / EXPAND / MACRO_COUNT
// 这组预处理工具宏。它们是 Logger 的 LogUtil.h 里"按参数个数选择宏"（LogLimitTime_1 /
// _2、Log_Unique_Timer_1 / _2）机制的基础，所以这里的断言同时也在保护那些日志宏。

#include <string>

#include <gtest/gtest.h>

#include "Base/MacroTool.h"

#define SV_TEST_NUMBER 42

namespace
{
// 用于验证 CAT 真的能拼出可用的标识符。
int myVariable = 0;
}  // namespace

// ISTR 是最内层：直接对 token 做字符串化，不展开宏。
TEST(MacroToolTest, IstrStringifiesWithoutExpanding)
{
    EXPECT_STREQ(ISTR(SV_TEST_NUMBER), "SV_TEST_NUMBER");
    EXPECT_STREQ(ISTR(hello world), "hello world");
}

// STR 先展开参数、再字符串化，所以宏参数会被替换成它的定义。
TEST(MacroToolTest, StrExpandsBeforeStringifying)
{
    EXPECT_STREQ(STR(SV_TEST_NUMBER), "42");
}

// CAT 先展开再拼接，能生成可用的标识符。
TEST(MacroToolTest, CatBuildsUsableIdentifier)
{
    CAT(my, Variable) = 7;
    EXPECT_EQ(myVariable, 7);
}

// EXPAND 把参数原样转发；MACRO_COUNT 依赖它吃掉 HELPER 宏多出的形参。
TEST(MacroToolTest, ExpandForwardsArgumentsVerbatim)
{
    const int values[] = {EXPAND(1, 2, 3)};
    EXPECT_EQ(sizeof(values) / sizeof(values[0]), 3u);
    EXPECT_EQ(values[2], 3);
}

// MACRO_COUNT 最多支持 9 个参数，边界值 1 与 9 都要正确。
TEST(MacroToolTest, MacroCountCountsArguments)
{
    EXPECT_EQ(MACRO_COUNT(a), 1);
    EXPECT_EQ(MACRO_COUNT(a, b), 2);
    EXPECT_EQ(MACRO_COUNT(a, b, c), 3);
    EXPECT_EQ(MACRO_COUNT(a, b, c, d, e), 5);
    EXPECT_EQ(MACRO_COUNT(a, b, c, d, e, f, g, h, i), 9);
}
