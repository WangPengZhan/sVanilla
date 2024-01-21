#pragma once
#include <string>
#include <vector>
#include <list>
#include <functional>

#include <zip.h>

class ResourceHelper
{
public:
    ResourceHelper() = default;
    ResourceHelper(const std::function<void(void)>& fn);
    ResourceHelper(const std::list<std::function<void(void)>>& fns);

    ~ResourceHelper();

    void addFn(const std::function<void(void)>& fn);
    void removeFn(const std::function<void(void)>& fn);

private:
    std::list<std::function<void(void)>> m_fns;
};

class Zipper
{
public:
    Zipper() = default;
    Zipper(const std::vector<std::string>& vectZipFiles, std::string strOutputFileName);
    Zipper(std::string strZipPath, std::string strOutputFileName, std::string strRelativePath = "");
    ~Zipper() = default;

    void setZipFiles(const std::vector<std::string>& vectImportFiles);
    const std::vector<std::string>& zipFiles() const;

    void setZipPath(const std::string& strZipPath);
    const std::string& zipPath() const;

    void setOutputFileName(const std::string& filename);
    const std::string& outputFileName() const;

    bool zip();

    static bool addFileToZip(zipFile zfile, const std::string& fileNameinZip, const std::string& srcfile);
    static bool collectFileInDirToZip(zipFile zfile, const std::string& filepath, const std::string& parentdirName);

private:
    std::vector<std::string> m_vectZipFiles;
    std::string m_strZipPath;
    std::string m_strOutputFileName;
    std::string m_strRelativePath;
};

class Unzipper
{
public:
    Unzipper() = default;
    Unzipper(std::string strUnzipperFile, std::string strPath);
    ~Unzipper() = default;

    void setUnzippedFile(const std::string& strUnzippedFile);
    const std::string& unzippedFile() const;

    void setOutputPath(const std::string& strPath);
    const std::string& outputPath() const;

    bool unzip();

private:
    std::string m_strUnzippedFile;
    std::string m_strOutputPath;
};
