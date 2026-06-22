#include <unzip.h>
#include <zip.h>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <list>
#include <utility>
#include <cstring>
#include <algorithm>
#include <cctype>
#include <cstdint>
#include <limits>
#include <vector>

#include "Zipper.h"

namespace
{
constexpr std::size_t maxEntryNameSize = std::numeric_limits<std::uint16_t>::max();
constexpr std::uint64_t maxCompressionRatio = 1000;
constexpr std::size_t readBufferSize = 1024 * 1024;
constexpr std::uint64_t zip64SizeThreshold = std::numeric_limits<std::uint32_t>::max();

bool isSymlink(const unz_file_info64& fileInfo)
{
    constexpr std::uint32_t unixFileTypeMask = 0170000;
    constexpr std::uint32_t unixSymlinkType = 0120000;
    const auto unixMode = static_cast<std::uint32_t>(fileInfo.external_fa >> 16);
    return (unixMode & unixFileTypeMask) == unixSymlinkType;
}

bool hasParentComponent(const std::filesystem::path& path)
{
    return std::any_of(path.begin(), path.end(), [](const auto& component) {
        return component == "..";
    });
}

bool hasWindowsDrivePrefix(const std::string& entryName)
{
    if (entryName.size() < 2 || entryName[1] != ':')
    {
        return false;
    }
    return std::isalpha(static_cast<unsigned char>(entryName[0])) != 0;
}

bool validEntryInfo(const unz_file_info64& fileInfo)
{
    if (fileInfo.size_filename == 0 || fileInfo.size_filename > maxEntryNameSize)
    {
        return false;
    }
    if (isSymlink(fileInfo))
    {
        return false;
    }
    if (fileInfo.uncompressed_size == 0)
    {
        return true;
    }
    if (fileInfo.compressed_size == 0)
    {
        return false;
    }
    return fileInfo.uncompressed_size / fileInfo.compressed_size <= maxCompressionRatio;
}

bool safeOutputPath(const std::filesystem::path& outputRoot, std::string entryName, std::filesystem::path& outputPath)
{
    std::replace(entryName.begin(), entryName.end(), '\\', '/');
    if (entryName.empty() || entryName.front() == '/')
    {
        return false;
    }
    if (hasWindowsDrivePrefix(entryName))
    {
        return false;
    }

    const std::filesystem::path relativePath(entryName);
    if (relativePath.is_absolute() || relativePath.has_root_path())
    {
        return false;
    }
    if (hasParentComponent(relativePath))
    {
        return false;
    }

    std::error_code error;
    outputPath = std::filesystem::weakly_canonical(outputRoot / relativePath, error);
    if (error)
    {
        return false;
    }

    const auto relativeOutput = outputPath.lexically_relative(outputRoot);
    if (relativeOutput.empty() || relativeOutput.is_absolute())
    {
        return false;
    }
    return !hasParentComponent(relativeOutput);
}
}  // namespace

ResourceHelper::ResourceHelper(const std::function<void(void)>& fn)
{
    m_fns.push_back(fn);
}

ResourceHelper::ResourceHelper(const std::list<std::function<void(void)>>& fns)
    : m_fns(fns)
{
}

ResourceHelper::~ResourceHelper()
{
    for (const auto& fn : m_fns)
    {
        if (fn)
        {
            fn();
        }
    }
}

void ResourceHelper::addFn(const std::function<void(void)>& fn)
{
    m_fns.push_back(fn);
}

Zipper::Zipper(const std::vector<std::string>& vectZipFiles, std::string strOutputFileName)
    : m_vectZipFiles(vectZipFiles)
    , m_strOutputFileName(std::move(strOutputFileName))
{
}

Zipper::Zipper(std::string strZipPath, std::string strOutputFileName, std::string strRelativePath)
    : m_strZipPath(std::move(strZipPath))
    , m_strOutputFileName(std::move(strOutputFileName))
    , m_strRelativePath(std::move(strRelativePath))
{
}

void Zipper::setZipFiles(const std::vector<std::string>& vectImportFiles)
{
    m_strZipPath.clear();
    m_strRelativePath.clear();
    m_vectZipFiles = vectImportFiles;
}

const std::vector<std::string>& Zipper::zipFiles() const
{
    return m_vectZipFiles;
}

void Zipper::setZipPath(const std::string& strZipPath)
{
    m_strZipPath = strZipPath;
}

const std::string& Zipper::zipPath() const
{
    return m_strZipPath;
}

void Zipper::setOutputFileName(const std::string& filename)
{
    m_strOutputFileName = filename;
}

const std::string& Zipper::outputFileName() const
{
    return m_strOutputFileName;
}

bool Zipper::zip()
{
    if (m_strOutputFileName.empty())
    {
        return false;
    }

    zipFile zFile = zipOpen64(m_strOutputFileName.c_str(), APPEND_STATUS_CREATE);
    if (nullptr == zFile)
    {
        return false;
    }

    ResourceHelper resurceHelper([zFile]() {
        zipClose(zFile, nullptr);
    });

    return collectFileInDirToZip(zFile, m_strZipPath, m_strZipPath);
}

bool Zipper::addFileToZip(zipFile zfile, const std::string& fileNameinZip, const std::string& srcfile)
{
    if (nullptr == zfile || fileNameinZip.empty())
    {
        return false;
    }

    std::time_t t_time = time(nullptr);
    std::tm* s_time = std::localtime(&t_time);
    zip_fileinfo zinfo = {0};
    tm_zip tmz = {s_time->tm_sec, s_time->tm_min, s_time->tm_hour, s_time->tm_mday, s_time->tm_mon, s_time->tm_year};
    zinfo.tmz_date = tmz;
    zinfo.dosDate = 0;
    zinfo.internal_fa = 0;
    zinfo.external_fa = 0;

    std::string strFileName = fileNameinZip;
    if (srcfile.empty() && !(strFileName.back() == '/' || strFileName.back() == '\\'))
    {
        strFileName += "/";
    }

    std::error_code error;
    const auto fileSize = srcfile.empty() ? 0 : std::filesystem::file_size(srcfile, error);
    if (error)
    {
        return false;
    }
    const int zip64 = fileSize >= zip64SizeThreshold ? 1 : 0;
    int nErr = zipOpenNewFileInZip64(zfile, strFileName.c_str(), &zinfo, nullptr, 0, nullptr, 0, nullptr, Z_DEFLATED, Z_DEFAULT_COMPRESSION, zip64);
    if (nErr != ZIP_OK)
    {
        return false;
    }

    if (!srcfile.empty())
    {
        // 打开源文件
        std::ifstream stream(srcfile, std::ios::binary);
        if (!stream.is_open() || stream.bad())
        {
            return false;
        }

        // 读入源文件写入zip文件
        int numBytes = 0;
        std::unique_ptr<char[]> pBuf(new char[1024 * 1024]);
        while (!stream.eof())
        {
            memset(pBuf.get(), 0x00, 1024 * 1024);
            stream.read(pBuf.get(), 1024 * 1024);
            numBytes = stream.gcount();
            zipWriteInFileInZip(zfile, pBuf.get(), numBytes);
        }
    }
    zipCloseFileInZip(zfile);

    return true;
}

bool Zipper::collectFileInDirToZip(zipFile zfile, const std::string& filepath, const std::string& parentdirName)
{
    if (nullptr == zfile || filepath.empty())
    {
        return false;
    }

    const std::filesystem::path& filePath = filepath;
    if (parentdirName.empty())
    {
        std::filesystem::current_path(filePath.parent_path());
    }
    else
    {
        std::filesystem::current_path(parentdirName);
    }

    for (const auto& entry : std::filesystem::recursive_directory_iterator(filepath))
    {
        std::string strAbsolute = std::filesystem::absolute(entry.path()).string();
        std::string strRelative = std::filesystem::relative(entry.path()).string();
        if (entry.is_directory())
        {
            addFileToZip(zfile, strRelative, "");
        }
        else
        {
            addFileToZip(zfile, strRelative, strAbsolute);
        }
    }

    return true;
}

Unzipper::Unzipper(std::string strUnzipperFile, std::string strPath)
    : m_strUnzippedFile(std::move(strUnzipperFile))
    , m_strOutputPath(std::move(strPath))
{
}

void Unzipper::setUnzippedFile(const std::string& strUnzippedFile)
{
    m_strUnzippedFile = strUnzippedFile;
}

const std::string& Unzipper::unzippedFile() const
{
    return m_strUnzippedFile;
}

void Unzipper::setOutputPath(const std::string& strPath)
{
    m_strOutputPath = strPath;
}

const std::string& Unzipper::outputPath() const
{
    return m_strOutputPath;
}

bool Unzipper::unzip()
{
    std::error_code error;
    if (m_strUnzippedFile.empty() || m_strOutputPath.empty())
    {
        return false;
    }
    if (!std::filesystem::is_regular_file(m_strUnzippedFile, error) || error)
    {
        return false;
    }

    const std::filesystem::path requestedOutputPath(m_strOutputPath);
    if (std::filesystem::exists(requestedOutputPath, error))
    {
        if (error || !std::filesystem::is_directory(requestedOutputPath, error))
        {
            return false;
        }
    }
    else if (!std::filesystem::create_directories(requestedOutputPath, error) || error)
    {
        return false;
    }

    const auto outputRoot = std::filesystem::weakly_canonical(requestedOutputPath, error);
    if (error)
    {
        return false;
    }

    unzFile unzfile = unzOpen64(m_strUnzippedFile.c_str());
    if (unzfile == nullptr)
    {
        return false;
    }
    ResourceHelper resource([unzfile]() {
        unzClose(unzfile);
    });

    unz_global_info64 globalInfo{};
    if (unzGetGlobalInfo64(unzfile, &globalInfo) != UNZ_OK)
    {
        return false;
    }

    std::vector<char> readBuffer(readBufferSize);
    for (std::uint64_t i = 0; i < globalInfo.number_entry; ++i)
    {
        unz_file_info64 fileInfo{};
        if (unzGetCurrentFileInfo64(unzfile, &fileInfo, nullptr, 0, nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            return false;
        }
        if (!validEntryInfo(fileInfo))
        {
            return false;
        }

        std::vector<char> entryNameBuffer(static_cast<std::size_t>(fileInfo.size_filename) + 1, '\0');
        if (unzGetCurrentFileInfo64(unzfile, &fileInfo, entryNameBuffer.data(), static_cast<uLong>(entryNameBuffer.size()), nullptr, 0, nullptr, 0) != UNZ_OK)
        {
            return false;
        }

        const std::string entryName(entryNameBuffer.data(), fileInfo.size_filename);
        const bool isDirectory = entryName.back() == '/' || entryName.back() == '\\';
        std::filesystem::path outputPath;
        if (!safeOutputPath(outputRoot, entryName, outputPath))
        {
            return false;
        }

        if (isDirectory)
        {
            if (!std::filesystem::create_directories(outputPath, error) && error)
            {
                return false;
            }
        }
        else
        {
            if (!std::filesystem::create_directories(outputPath.parent_path(), error) && error)
            {
                return false;
            }

            std::ofstream stream(outputPath, std::ios::out | std::ios::binary | std::ios::trunc);
            if (!stream)
            {
                return false;
            }
            if (unzOpenCurrentFile(unzfile) != UNZ_OK)
            {
                return false;
            }

            bool succeeded = true;
            std::uint64_t writtenSize = 0;
            while (true)
            {
                const int readSize = unzReadCurrentFile(unzfile, readBuffer.data(), static_cast<unsigned int>(readBuffer.size()));
                if (readSize < 0)
                {
                    succeeded = false;
                    break;
                }
                if (readSize == 0)
                {
                    break;
                }

                writtenSize += static_cast<std::uint64_t>(readSize);
                if (writtenSize > fileInfo.uncompressed_size)
                {
                    succeeded = false;
                    break;
                }
                stream.write(readBuffer.data(), readSize);
                succeeded = succeeded && stream.good();
            }

            stream.close();
            succeeded = succeeded && unzCloseCurrentFile(unzfile) == UNZ_OK && writtenSize == fileInfo.uncompressed_size;
            if (!succeeded)
            {
                std::filesystem::remove(outputPath, error);
                return false;
            }
        }

        if (i + 1 < globalInfo.number_entry && unzGoToNextFile(unzfile) != UNZ_OK)
        {
            return false;
        }
    }

    return true;
}
