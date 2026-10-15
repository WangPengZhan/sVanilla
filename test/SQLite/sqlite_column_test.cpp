#include "sqlite_fixture.h"

#include <limits>
#include <string>
#include <vector>

namespace sqlite_test
{

TEST(SqliteTest, ColumnMapsVariantIndexToFieldType)
{
    EXPECT_EQ(sqlite::SqliteColumn(std::monostate{}).getType(), sqlite::FieldType::Null);
    EXPECT_EQ(sqlite::SqliteColumn(int64_t{1}).getType(), sqlite::FieldType::Integer);
    EXPECT_EQ(sqlite::SqliteColumn(1.5).getType(), sqlite::FieldType::Float);
    EXPECT_EQ(sqlite::SqliteColumn(std::string("x")).getType(), sqlite::FieldType::Text);
    EXPECT_EQ(sqlite::SqliteColumn(std::vector<uint8_t>{1, 2}).getType(), sqlite::FieldType::Blob);
}

TEST(SqliteTest, ColumnTypePredicatesAreMutuallyExclusive)
{
    const sqlite::SqliteColumn integer(int64_t{7});
    EXPECT_TRUE(integer.isInteger());
    EXPECT_FALSE(integer.isFloat());
    EXPECT_FALSE(integer.isText());
    EXPECT_FALSE(integer.isBlob());
    EXPECT_FALSE(integer.isNull());

    const sqlite::SqliteColumn text(std::string("7"));
    EXPECT_TRUE(text.isText());
    EXPECT_FALSE(text.isInteger());

    const sqlite::SqliteColumn null(std::monostate{});
    EXPECT_TRUE(null.isNull());
    EXPECT_FALSE(null.isInteger());
    EXPECT_FALSE(null.isText());
}

TEST(SqliteTest, NumericAccessorsReadArithmeticAlternatives)
{
    const sqlite::SqliteColumn integer(int64_t{42});
    EXPECT_EQ(integer.getInt(), 42);
    EXPECT_EQ(integer.getInt64(), 42);
    EXPECT_EQ(integer.getUInt(), 42u);
    EXPECT_DOUBLE_EQ(integer.getDouble(), 42.0);

    const sqlite::SqliteColumn real(2.5);
    EXPECT_DOUBLE_EQ(real.getDouble(), 2.5);
    // 浮点值也能被整数访问器读取（截断）
    EXPECT_EQ(real.getInt(), 2);

    // 越界：int64 最大值截到 int32 由 static_cast 决定，不抛异常
    const sqlite::SqliteColumn big(int64_t{std::numeric_limits<int64_t>::max()});
    EXPECT_EQ(big.getInt64(), std::numeric_limits<int64_t>::max());
}

TEST(SqliteTest, NumericAccessorsReturnZeroForNonArithmetic)
{
    const sqlite::SqliteColumn text(std::string("42"));
    EXPECT_EQ(text.getInt(), 0);
    EXPECT_EQ(text.getInt64(), 0);
    EXPECT_EQ(text.getUInt(), 0u);
    EXPECT_DOUBLE_EQ(text.getDouble(), 0.0);

    const sqlite::SqliteColumn null(std::monostate{});
    EXPECT_EQ(null.getInt(), 0);
    EXPECT_EQ(null.getInt64(), 0);
}

TEST(SqliteTest, StringAccessorsRenderScalarsAndNulls)
{
    EXPECT_EQ(sqlite::SqliteColumn(int64_t{42}).getString(), "42");
    EXPECT_EQ(sqlite::SqliteColumn(2.5).getString(), "2.500000");
    EXPECT_EQ(sqlite::SqliteColumn(std::string("hello")).getString(), "hello");
    EXPECT_EQ(sqlite::SqliteColumn(std::monostate{}).getString(), "");

    EXPECT_EQ(sqlite::SqliteColumn(int64_t{42}).getText("fallback"), "42");
    EXPECT_EQ(sqlite::SqliteColumn(std::string("hello")).getText("fallback"), "hello");
}

TEST(SqliteTest, StringAccessorsAreNotDefeatedByCvQualification)
{
    const sqlite::SqliteColumn integer(int64_t{-1234567890123LL});
    EXPECT_EQ(integer.getString(), std::to_string(-1234567890123LL));

    const sqlite::SqliteColumn real(1.25);
    EXPECT_EQ(real.getString(), std::to_string(1.25));
}

TEST(SqliteTest, BlobToTextKeepsEmbeddedNulBytes)
{
    const std::vector<uint8_t> raw{0x41, 0x00, 0x42, 0x00, 0x00, 0x43};
    const sqlite::SqliteColumn blob(raw);

    EXPECT_TRUE(blob.isBlob());
    EXPECT_EQ(blob.getBlob(), raw);

    const std::string rendered = blob.getString();
    ASSERT_EQ(rendered.size(), raw.size());
    EXPECT_EQ(rendered, std::string(reinterpret_cast<const char*>(raw.data()), raw.size()));
    EXPECT_EQ(blob.getBytes(), static_cast<int>(raw.size()));
}

TEST(SqliteTest, EmptyBlobRendersEmpty)
{
    const sqlite::SqliteColumn blob(std::vector<uint8_t>{});
    EXPECT_TRUE(blob.isBlob());
    EXPECT_TRUE(blob.getBlob().empty());
    EXPECT_TRUE(blob.getString().empty());
    EXPECT_EQ(blob.getBytes(), 0);
}

TEST(SqliteTest, ByteSizeReportsStorageWidth)
{
    EXPECT_EQ(sqlite::SqliteColumn(int64_t{1}).getBytes(), static_cast<int>(sizeof(int64_t)));
    EXPECT_EQ(sqlite::SqliteColumn(1.0).getBytes(), static_cast<int>(sizeof(double)));
    EXPECT_EQ(sqlite::SqliteColumn(std::string("abcd")).getBytes(), 4);
    EXPECT_EQ(sqlite::SqliteColumn(std::monostate{}).getBytes(), 0);
    EXPECT_EQ(sqlite::SqliteColumn(std::string("abcd")).size(), 4);
}

TEST(SqliteTest, ConversionOperatorsMirrorAccessors)
{
    const sqlite::SqliteColumn integer(int64_t{65});
    EXPECT_EQ(static_cast<int64_t>(integer), 65);
    EXPECT_EQ(static_cast<int32_t>(integer), 65);
    EXPECT_EQ(static_cast<uint32_t>(integer), 65u);
    EXPECT_EQ(static_cast<char>(integer), 'A');
    EXPECT_EQ(static_cast<int8_t>(integer), 65);

    const sqlite::SqliteColumn text(std::string("abc"));
    EXPECT_STREQ(static_cast<const char*>(text), "abc");
    EXPECT_EQ(text.getString(), "abc");

    const std::vector<uint8_t> raw{9, 8, 7};
    const sqlite::SqliteColumn blob(raw);
    EXPECT_EQ(static_cast<std::vector<uint8_t>>(blob), raw);
}

TEST(SqliteTest, ColumnCarriesNameAndOriginName)
{
    const sqlite::SqliteColumn column(int64_t{1}, 3, "alias", "origin");
    EXPECT_EQ(column.getName(), "alias");
    EXPECT_EQ(column.getOriginName(), "origin");

    const sqlite::SqliteColumn plain(int64_t{1});
    EXPECT_TRUE(plain.getName().empty());
    EXPECT_TRUE(plain.getOriginName().empty());
}

}  // namespace sqlite_test
