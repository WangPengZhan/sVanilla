#include <atomic>
#include <chrono>
#include <filesystem>
#include <fstream>
#include <functional>
#include <iterator>
#include <list>
#include <string>
#include <system_error>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <minizip/zip.h>

#include "Zipper/Zipper.h"

namespace
{

// 用例独占的临时目录，析构时整体删除，避免在系统 temp 里留下垃圾。
class ScopedTempDir
{
public:
    explicit ScopedTempDir(std::string tag)
    {
        // runId 保证不同次运行不会复用同一目录：上一轮可能残留没删干净的文件。
        static const std::string runId = std::to_string(std::chrono::steady_clock::now().time_since_epoch().count());
        static std::atomic<unsigned> sequence{0};
        m_path = std::filesystem::temp_directory_path() / ("svanilla_" + tag + "_" + runId + "_" + std::to_string(sequence++));
        std::filesystem::create_directories(m_path);
    }

    ScopedTempDir(const ScopedTempDir&) = delete;
    ScopedTempDir& operator=(const ScopedTempDir&) = delete;

    ~ScopedTempDir()
    {
        std::error_code error;
        std::filesystem::remove_all(m_path, error);
    }

    const std::filesystem::path& path() const
    {
        return m_path;
    }

private:
    std::filesystem::path m_path;
};

std::string readAll(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    if (!stream)
    {
        return {};
    }
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}

bool createZip(const std::filesystem::path& zipPath, const std::vector<std::pair<std::string, std::string>>& entries, bool forceZip64 = false)
{
    zipFile archive = zipOpen64(zipPath.string().c_str(), APPEND_STATUS_CREATE);
    if (!archive)
    {
        return false;
    }

    bool succeeded = true;
    for (const auto& [name, content] : entries)
    {
        zip_fileinfo fileInfo{};
        const int zip64 = forceZip64 ? 1 : 0;
        if (zipOpenNewFileInZip64(archive, name.c_str(), &fileInfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, zip64) != ZIP_OK)
        {
            succeeded = false;
            break;
        }
        if (!content.empty() && zipWriteInFileInZip(archive, content.data(), static_cast<unsigned int>(content.size())) != ZIP_OK)
        {
            succeeded = false;
        }
        if (zipCloseFileInZip(archive) != ZIP_OK)
        {
            succeeded = false;
        }
        if (!succeeded)
        {
            break;
        }
    }

    return zipClose(archive, nullptr) == ZIP_OK && succeeded;
}

void writeFile(const std::filesystem::path& path, const std::string& content)
{
    std::filesystem::create_directories(path.parent_path());
    std::ofstream stream(path, std::ios::binary | std::ios::trunc);
    stream.write(content.data(), static_cast<std::streamsize>(content.size()));
}

}  // namespace

// ------------------------------------------------------------------- Unzipper

TEST(UnzipperTest, CreatesOutputDirectoryAndExtractsFile)
{
    ScopedTempDir temporaryDirectory("unzip");

    const auto archive = temporaryDirectory.path() / "normal.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"nested/file.txt", "content"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_TRUE(unzipper.unzip());
    EXPECT_EQ(readAll(output / "nested/file.txt"), "content");
}

TEST(UnzipperTest, ExtractsEveryEntry)
{
    ScopedTempDir temporaryDirectory("unzip_multi");

    const auto archive = temporaryDirectory.path() / "multi.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"one.txt",   "1"  },
                                       {"two.txt",   "22" },
                                       {"three.txt", "333"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    ASSERT_TRUE(unzipper.unzip());
    EXPECT_EQ(readAll(output / "one.txt"), "1");
    EXPECT_EQ(readAll(output / "two.txt"), "22");
    EXPECT_EQ(readAll(output / "three.txt"), "333");
}

TEST(UnzipperTest, ExtractsZip64Entry)
{
    ScopedTempDir temporaryDirectory("unzip_zip64");

    const auto archive = temporaryDirectory.path() / "zip64.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive,
                          {
                              {"large-file-metadata.txt", "content"}
    },
                          true));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_TRUE(unzipper.unzip());
    EXPECT_EQ(readAll(output / "large-file-metadata.txt"), "content");
}

TEST(UnzipperTest, RejectsParentDirectoryTraversal)
{
    ScopedTempDir temporaryDirectory("unzip_traversal");

    const auto archive = temporaryDirectory.path() / "traversal.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"../outside.txt", "unexpected"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_FALSE(unzipper.unzip());
    EXPECT_FALSE(std::filesystem::exists(temporaryDirectory.path() / "outside.txt"));
}

TEST(UnzipperTest, RejectsBackslashTraversal)
{
    ScopedTempDir temporaryDirectory("unzip_backslash");

    const auto archive = temporaryDirectory.path() / "backslash.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"..\\outside.txt", "unexpected"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_FALSE(unzipper.unzip());
    EXPECT_FALSE(std::filesystem::exists(temporaryDirectory.path() / "outside.txt"));
}

TEST(UnzipperTest, RejectsAbsoluteWindowsPath)
{
    ScopedTempDir temporaryDirectory("unzip_absolute");

    const auto archive = temporaryDirectory.path() / "absolute.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"C:/outside.txt", "unexpected"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_FALSE(unzipper.unzip());
}

TEST(UnzipperTest, RejectsMissingArchive)
{
    ScopedTempDir temporaryDirectory("unzip_missing");

    Unzipper unzipper((temporaryDirectory.path() / "absent.zip").string(), (temporaryDirectory.path() / "output").string());
    EXPECT_FALSE(unzipper.unzip());
}

TEST(UnzipperTest, RejectsEmptyArguments)
{
    Unzipper unzipper;
    EXPECT_FALSE(unzipper.unzip());

    Unzipper withoutOutput("archive.zip", "");
    EXPECT_FALSE(withoutOutput.unzip());

    Unzipper withoutArchive("", "output");
    EXPECT_FALSE(withoutArchive.unzip());
}

TEST(UnzipperTest, RejectsOutputPathThatIsARegularFile)
{
    ScopedTempDir temporaryDirectory("unzip_blocked");

    const auto archive = temporaryDirectory.path() / "blocked.zip";
    ASSERT_TRUE(createZip(archive, {
                                       {"file.txt", "content"}
    }));

    const auto blocked = temporaryDirectory.path() / "blocked";
    writeFile(blocked, "not a directory");

    Unzipper unzipper(archive.string(), blocked.string());
    EXPECT_FALSE(unzipper.unzip());
}

TEST(UnzipperTest, SettersAndGettersRoundTrip)
{
    Unzipper unzipper;
    unzipper.setUnzippedFile("archive.zip");
    unzipper.setOutputPath("output");

    EXPECT_EQ(unzipper.unzippedFile(), "archive.zip");
    EXPECT_EQ(unzipper.outputPath(), "output");
}

// --------------------------------------------------------------------- Zipper

TEST(ZipperTest, PackagesDirectoryContentsAndRestoresThemWithUnzipper)
{
    ScopedTempDir temporaryDirectory("zip_roundtrip");

    const auto source = temporaryDirectory.path() / "src";
    writeFile(source / "alpha.txt", "alpha");
    writeFile(source / "nested" / "beta.txt", "beta");

    const auto archive = temporaryDirectory.path() / "bundle.zip";
    Zipper zipper(source.string(), archive.string());
    ASSERT_TRUE(zipper.zip());
    ASSERT_TRUE(std::filesystem::exists(archive));

    const auto restored = temporaryDirectory.path() / "restored";
    Unzipper unzipper(archive.string(), restored.string());
    ASSERT_TRUE(unzipper.unzip());

    EXPECT_EQ(readAll(restored / "alpha.txt"), "alpha");
    EXPECT_EQ(readAll(restored / "nested" / "beta.txt"), "beta");
}

TEST(ZipperTest, RejectsEmptyOutputFileName)
{
    ScopedTempDir temporaryDirectory("zip_nooutput");

    const auto source = temporaryDirectory.path() / "src";
    writeFile(source / "a.txt", "a");

    Zipper zipper(source.string(), "");
    EXPECT_FALSE(zipper.zip());
}

// 源目录不存在时返回 false，不再抛出 std::filesystem::filesystem_error。
// 旧行为（抛异常）由 ThrowsForMissingSourceDirectory 固化；collectFileInDirToZip 改为
// 带 std::error_code 的迭代器重载后，zip() 的失败只通过返回值表达。
TEST(ZipperTest, ReturnsFalseForMissingSourceDirectory)
{
    ScopedTempDir temporaryDirectory("zip_missingsrc");

    Zipper zipper((temporaryDirectory.path() / "absent").string(), (temporaryDirectory.path() / "out.zip").string());
    EXPECT_FALSE(zipper.zip());
}

TEST(ZipperTest, SettersAndGettersRoundTrip)
{
    ScopedTempDir temporaryDirectory("zip_accessors");

    const auto source = temporaryDirectory.path() / "src";
    const auto archive = temporaryDirectory.path() / "out.zip";

    Zipper zipper(source.string(), archive.string());
    EXPECT_EQ(zipper.zipPath(), source.string());
    EXPECT_EQ(zipper.outputFileName(), archive.string());

    zipper.setOutputFileName("renamed.zip");
    EXPECT_EQ(zipper.outputFileName(), "renamed.zip");

    zipper.setZipFiles({"a.txt", "b.txt"});
    EXPECT_EQ(zipper.zipFiles().size(), 2u);
    // setZipFiles 走的是"按文件列表打包"的分支，会清空目录模式的路径
    EXPECT_TRUE(zipper.zipPath().empty());
    EXPECT_EQ(zipper.zipFiles().front(), "a.txt");
}

TEST(ZipperTest, AddFileToZipStoresSingleFile)
{
    ScopedTempDir temporaryDirectory("zip_addfile");

    const auto payload = temporaryDirectory.path() / "payload.bin";
    const std::string content = "0123456789";
    writeFile(payload, content);

    const auto archive = temporaryDirectory.path() / "single.zip";
    zipFile handle = zipOpen64(archive.string().c_str(), APPEND_STATUS_CREATE);
    ASSERT_NE(handle, nullptr);

    EXPECT_TRUE(Zipper::addFileToZip(handle, "payload.bin", payload.string()));
    ASSERT_EQ(zipClose(handle, nullptr), ZIP_OK);

    const auto restored = temporaryDirectory.path() / "out";
    Unzipper unzipper(archive.string(), restored.string());
    ASSERT_TRUE(unzipper.unzip());
    EXPECT_EQ(readAll(restored / "payload.bin"), content);
}

TEST(ZipperTest, AddFileToZipRejectsInvalidArguments)
{
    ScopedTempDir temporaryDirectory("zip_addfile_bad");

    const auto payload = temporaryDirectory.path() / "p.bin";
    writeFile(payload, "x");

    zipFile handle = zipOpen64((temporaryDirectory.path() / "archive.zip").string().c_str(), APPEND_STATUS_CREATE);
    ASSERT_NE(handle, nullptr);

    EXPECT_FALSE(Zipper::addFileToZip(nullptr, "name", payload.string()));
    EXPECT_FALSE(Zipper::addFileToZip(handle, "", payload.string()));
    EXPECT_FALSE(Zipper::addFileToZip(handle, "name", (temporaryDirectory.path() / "absent.bin").string()));

    EXPECT_EQ(zipClose(handle, nullptr), ZIP_OK);
}

// ------------------------------------------------------------ ResourceHelper

TEST(ResourceHelperTest, RunsCallbackOnDestruction)
{
    int calls = 0;
    {
        ResourceHelper helper([&calls]() {
            ++calls;
        });
        EXPECT_EQ(calls, 0);
    }
    EXPECT_EQ(calls, 1);
}

TEST(ResourceHelperTest, AddFnAppendsAdditionalCallbacks)
{
    int calls = 0;
    {
        ResourceHelper helper([&calls]() {
            ++calls;
        });
        helper.addFn([&calls]() {
            ++calls;
        });
        EXPECT_EQ(calls, 0);
    }
    EXPECT_EQ(calls, 2);
}

TEST(ResourceHelperTest, RunsCallbackListInOrder)
{
    std::vector<int> order;
    {
        ResourceHelper helper(std::list<std::function<void(void)>>{
            [&order]() {
                order.push_back(1);
            },
            [&order]() {
                order.push_back(2);
            },
            [&order]() {
                order.push_back(3);
            },
        });
    }
    EXPECT_EQ(order, (std::vector<int>{1, 2, 3}));
}

TEST(ResourceHelperTest, SkipsEmptyCallbacks)
{
    int calls = 0;
    {
        ResourceHelper helper(std::list<std::function<void(void)>>{std::function<void(void)>{}, [&calls]() {
                                                                       ++calls;
                                                                   }});
    }
    EXPECT_EQ(calls, 1);
}
