#include <chrono>
#include <filesystem>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

#include <gtest/gtest.h>
#include <minizip/zip.h>

#include "Zipper/Zipper.h"

namespace
{
class TemporaryDirectory
{
public:
    TemporaryDirectory()
    {
        const auto suffix = std::chrono::steady_clock::now().time_since_epoch().count();
        m_path = std::filesystem::current_path() / ("svanilla_zipper_test_" + std::to_string(suffix));
        std::filesystem::create_directories(m_path);
    }

    ~TemporaryDirectory()
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

std::string readFile(const std::filesystem::path& path)
{
    std::ifstream stream(path, std::ios::binary);
    return {std::istreambuf_iterator<char>(stream), std::istreambuf_iterator<char>()};
}
}  // namespace

TEST(UnzipperTest, CreatesOutputDirectoryAndExtractsFile)
{
    TemporaryDirectory temporaryDirectory;
    const auto archive = temporaryDirectory.path() / "normal.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"nested/file.txt", "content"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_TRUE(unzipper.unzip());
    EXPECT_EQ(readFile(output / "nested/file.txt"), "content");
}

TEST(UnzipperTest, ExtractsZip64Entry)
{
    TemporaryDirectory temporaryDirectory;
    const auto archive = temporaryDirectory.path() / "zip64.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive,
                          {
                              {"large-file-metadata.txt", "content"}
    },
                          true));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_TRUE(unzipper.unzip());
    EXPECT_EQ(readFile(output / "large-file-metadata.txt"), "content");
}

TEST(UnzipperTest, RejectsParentDirectoryTraversal)
{
    TemporaryDirectory temporaryDirectory;
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
    TemporaryDirectory temporaryDirectory;
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
    TemporaryDirectory temporaryDirectory;
    const auto archive = temporaryDirectory.path() / "absolute.zip";
    const auto output = temporaryDirectory.path() / "output";
    ASSERT_TRUE(createZip(archive, {
                                       {"C:/outside.txt", "unexpected"}
    }));

    Unzipper unzipper(archive.string(), output.string());
    EXPECT_FALSE(unzipper.unzip());
}
