#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>

#include <memory>

#include "AriaServer.h"
#include "Aria2Net/AriaLog.h"

namespace aria2net
{

AriaServer::AriaServer() : m_aria2Process(std::make_unique<QProcess>())
{
}

AriaServer::~AriaServer()
{
    forceCloseServer();
}

void AriaServer::startServerAsync()
{
    m_future = std::async(std::launch::async, [&]() {
        QString ariaPath = QApplication::applicationDirPath() + "/aria/";
        QString ariaExecutable = QStandardPaths::findExecutable("aria2c", QStringList() << ariaPath);
        QString sessionFile = ariaPath + "aira.session";
        QString logFile = ariaPath + "log.txt";

        QDir airDir = QDir(ariaPath);

        // 设置启动的程序名和命令行参数
        m_aria2Process->setProgram(ariaExecutable);
        m_aria2Process->setArguments(QStringList() << "--enable-rpc"
                                                   << "--rpc-listen-all=true"
                                                   << "--rpc-allow-origin-all=true"
                                                   << "--rpc-listen-port=6800"
                                                   << "--rpc-secret=downkyi"
                                                   << "--input-file=" + sessionFile << "--save-session=" + sessionFile << "--save-session-interval=30"
                                                   << "--log=" + logFile << "--log-level=info"
                                                   << "--max-concurrent-downloads=3"
                                                   << "--max-connection-per-server=16"
                                                   << "--split=5"
                                                   << "--min-split-size=10M"
                                                   << "--max-overall-download-limit=0"
                                                   << "--max-download-limit=0"
                                                   << "--max-overall-upload-limit=0"
                                                   << "--max-upload-limit=0"
                                                   << "--continue=true"
                                                   << "--allow-overwrite=true"
                                                   << "--auto-file-renaming=false"
                                                   << "--file-allocation=none"
                                                   << "--header=\"Cookie: \""
                                                   << "--header=\"Origin: https://www.bilibili.com\""
                                                   << "--header=\"Referer: https://www.bilibili.com\""
                                                   << "--header=\"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
                                                      "like Gecko) Chrome/97.0.4692.99 Safari/537.36\"");

        if (m_aria2Process->state() != QProcess::Running)
        {
            m_aria2Process->start();
        }

        // 检查是否成功启动
        if (!m_aria2Process->waitForStarted())
        {
            ARIA_LOG_INFO("error starting aria2 process: {}", m_aria2Process->errorString().toStdString());
            return false;
        }
        else
        {
            ARIA_LOG_INFO("Aria2 process started successfully.");
            return true;
        }
    });
}

void AriaServer::closeServer()
{
    if (m_aria2Process && m_aria2Process->state() == QProcess::Running)
    {
        m_aria2Process->kill();
        m_aria2Process->waitForFinished();
    }
}

void AriaServer::forceCloseServer()
{
    if (m_aria2Process && m_aria2Process->state() == QProcess::Running)
    {
        m_aria2Process->kill();
        m_aria2Process->waitForFinished();
    }
}

void AriaServer::setErrorFunc(std::function<void()> func)
{
    m_errorFunc = std::move(func);
}

void AriaServer::setCloseFunc(std::function<void()> func)
{
    m_closeFunc = std::move(func);
}

}  // namespace aria2net
