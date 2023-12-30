#include <minizip/unzip.h>
#include <minizip/zip.h>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <list>

#include "Zipper.h"

constexpr int nMaxPath = 1024;

ResourceHelper::ResourceHelper(const std::function<void(void)>& fn)
{
    m_fns.push_back(fn);
}

ResourceHelper::ResourceHelper(const std::list<std::function<void(void)>>& fns) : m_fns(fns)
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

Zipper::Zipper(const std::vector<std::string>& vectZipFiles, const std::string& strOutputFileName)
    : m_vectZipFiles(vectZipFiles), m_strOutputFileName(strOutputFileName)
{
}

Zipper::Zipper(const std::string& strZipPath, const std::string& strOutputFileName, const std::string& strRelativePath)
    : m_strZipPath(strZipPath), m_strOutputFileName(strOutputFileName), m_strRelativePath(strRelativePath)
{
}

void Zipper::setZipFiles(const std::vector<std::string> vectImportFiles)
{
    m_strZipPath.clear();
    m_strRelativePath.clear();
    m_vectZipFiles = vectImportFiles;
}

const std::vector<std::string>& Zipper::zipFiles() const
{
    return m_vectZipFiles;
}

void Zipper::setZipPath(const std::string strZipPath)
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

    zipFile zFile = zipOpen(m_strOutputFileName.c_str(), APPEND_STATUS_CREATE);
    if (nullptr == zFile)
    {
        std::cout << "create zip file failed." << std::endl;
        return false;
    }

    ResourceHelper resurceHelper([zFile]() { zipClose(zFile, NULL); });

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

    int nErr = zipOpenNewFileInZip(zfile, strFileName.c_str(), &zinfo, NULL, 0, NULL, 0, NULL, Z_DEFLATED, Z_DEFAULT_COMPRESSION);
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
            std::cout << "Open source file failed." << std::endl;
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

    std::filesystem::path filePath = filePath;
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

Unzipper::Unzipper(const std::string& strUnzipperFile, const std::string& strPath) : m_strUnzippedFile(strUnzipperFile), m_strOutputPath(strPath)
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
    if (m_strUnzippedFile.empty() || !std::filesystem::exists(m_strUnzippedFile))
    {
        return false;
    }

    if (!std::filesystem::is_directory(m_strOutputPath))
    {
        return false;
    }

    if (!std::filesystem::exists(m_strOutputPath))
    {
        std::filesystem::create_directories(m_strOutputPath);
    }

    ResourceHelper resource;

    // 打开zip文件
    unzFile unzfile = unzOpen(m_strUnzippedFile.c_str());
    if (unzfile == nullptr)
    {
        return false;
    }
    resource.addFn([unzfile]() { unzCloseCurrentFile(unzfile); });

    // 获取zip文件的信息
    unz_global_info* pGlobalInfo = new unz_global_info;
    int nReturnValue = unzGetGlobalInfo(unzfile, pGlobalInfo);
    if (nReturnValue != UNZ_OK)
    {
        return false;
    }
    resource.addFn([pGlobalInfo]() { delete pGlobalInfo; });

    // 解析zip文件
    unz_file_info* pFileInfo = new unz_file_info;
    char szZipFName[nMaxPath] = {0};
    char szExtraName[nMaxPath] = {0};
    char szCommName[nMaxPath] = {0};
    resource.addFn([pFileInfo]() { delete pFileInfo; });

    // 存放从zip中解析出来的内部文件名
    for (int i = 0; i < pGlobalInfo->number_entry; ++i)
    {
        // 解析得到zip中的文件信息
        nReturnValue = unzGetCurrentFileInfo(unzfile, pFileInfo, szZipFName, nMaxPath, szExtraName, nMaxPath, szCommName, nMaxPath);
        if (nReturnValue != UNZ_OK)
        {
            return false;
        }

        std::cout << "ZipName: " << szZipFName << "; Extra: " << szExtraName << "; Comm: " << szCommName << std::endl;
        std::string strZipFileName = szZipFName;
        if (strZipFileName.empty())
        {
            continue;
        }
        bool bIsDir = strZipFileName.back() == '/' || strZipFileName.back() == '\\';
        std::filesystem::path strFullFilePath = m_strOutputPath + "/" + szZipFName;
        if (bIsDir)  // 创建目录
        {
            if (std::filesystem::exists(strFullFilePath))
            {
                std::filesystem::create_directory(strFullFilePath);
            }
        }
        else
        {
            // 创建文件
            if (!std::filesystem::exists(strFullFilePath.parent_path()))
            {
                std::filesystem::create_directories(strFullFilePath.parent_path());
            }
            std::ofstream stream(strFullFilePath, std::ios::out | std::ios::binary);

            // 打开文件
            nReturnValue = unzOpenCurrentFile(unzfile);
            if (nReturnValue != UNZ_OK)
            {
                return false;
            }

            // 读取文件
            uLong uFilesize = pFileInfo->uncompressed_size;
            std::unique_ptr<char[]> szReadBuffer(new char[uFilesize]);
            while (true)
            {
                memset(szReadBuffer.get(), 0, uFilesize);
                int nReadFileSize = unzReadCurrentFile(unzfile, szReadBuffer.get(), uFilesize);
                if (nReadFileSize < 0)  // 读取文件失败
                {
                    return false;
                }
                else if (nReadFileSize == 0)  // 读取文件完毕
                {
                    break;
                }
                else  // 写入读取的内容
                {
                    stream.write(szReadBuffer.get(), nReadFileSize);
                }
            }

            stream.close();
        }
        unzGoToNextFile(unzfile);
    }

    std::cout << "End unpacking the package... " << std::endl;
    return true;
}
