#include <QApplication>
#include <QFile>
#include <QDir>
#include <QDebug>

#include <chrono>
#include <thread>
#include <future>

#include <windows.h>

#include "AriaServer.h"
#include "Aria2Net/AriaLog.h"

namespace aria2net
{

AriaServer::AriaServer(QObject* parent)
    : QObject(parent)
    , m_pi{0}
{
}

AriaServer::~AriaServer()
{
    ForceCloseServer();
}

void AriaServer::StartServerAsync()
{
    ForceCloseServer();

    QString ariaPath = QApplication::applicationDirPath() + "/aria/";
    QString sessionFile = ariaPath + "aira.session";
    QString logFile = ariaPath + "log.txt";

    std::future<bool> result = std::async(std::launch::async, [&]() -> bool {
        QDir airDir = QDir(ariaPath);
        bool exist = airDir.exists();
        if (!exist)
        {
            bool ok = airDir.mkpath(ariaPath);
            if (!ok)
            {
                return ok;
            }
        }

        exist = QFile::exists(sessionFile);
        if (!exist)
        {
            QFile file(sessionFile);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            file.close();
        }

        exist = QFile::exists(logFile);
        if (!exist)
        {
            QFile file(logFile);
            file.open(QIODevice::ReadWrite | QIODevice::Text);
            file.close();
        }

        QString ariaExefilename = ariaPath + "aria2c.exe ";
        QString startAgr =
            "--enable-rpc --rpc-listen-all=true --rpc-allow-origin-all=true --rpc-listen-port=6800 --rpc-secret=downkyi"
            " --input-file=\"%1\" "
            "--save-session=\"%1\" "
            "--save-session-interval=30 --log=\"%2\" "
            "--log-level=info "
            "--max-concurrent-downloads=3 --max-connection-per-server=16 "
            "--split=5 --min-split-size=10M --max-overall-download-limit=0 "
            "--max-download-limit=0 --max-overall-upload-limit=0 --max-upload-limit=0 "
            "--continue=true --allow-overwrite=true --auto-file-renaming=false --file-allocation=none "
            "--header=\"Cookie: \" --header=\"Origin: https://www.bilibili.com\" "
            "--header=\"Referer: https://www.bilibili.com\" "
            "--header=\"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, like Gecko) Chrome/97.0.4692.99 Safari/537.36\"";
        startAgr = startAgr.arg(sessionFile).arg(logFile);
        auto ariaExeWFilename = ariaExefilename.toStdString();
        auto startWAgr = ariaExeWFilename + startAgr.toStdString();

        STARTUPINFO si;
        ZeroMemory(&si, sizeof(si));
        si.cb = sizeof(si);

        DWORD Flag = 0;
#ifdef _DEBUG
        Flag = CREATE_NEW_CONSOLE;
#else
        Flag = CREATE_NO_WINDOW;
#endif  //  _DEBUG

        if (!::CreateProcess((LPCSTR)ariaExeWFilename.c_str(), (LPSTR)startWAgr.c_str(), NULL, NULL, FALSE, Flag, NULL, NULL, &si, &m_pi))
        {
            qDebug() << QString("create aria2c.exe failed, error code: %1").arg(::GetLastError());
            ARIA_LOG_ERROR("create aria2c.exe failed, error code: %d", ::GetLastError());
            return false;
        }

        ARIA_LOG_INFO("create aria2c.exe success");
        return true;
    });
}

void AriaServer::CloseServer()
{
    if (m_pi.hThread != nullptr && m_pi.hThread != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_pi.hThread);
    }

    if (m_pi.hProcess != nullptr && m_pi.hProcess != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_pi.hProcess);
    }
    ARIA_LOG_INFO("close aria2c.exe handle");
}

void AriaServer::ForceCloseServer()
{
    if (m_pi.hProcess != nullptr && m_pi.hProcess != INVALID_HANDLE_VALUE)
    {
        ::TerminateProcess(m_pi.hProcess, 0);
    }

    CloseServer();
}

}  // namespace aria2net
