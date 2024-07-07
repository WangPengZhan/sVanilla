#include <QApplication>
#include <QStandardPaths>
#include <QDir>
#include <QProcess>

#include <memory>

#include "AriaServer.h"
#include "Aria2Net/AriaLog.h"
#include "SUI/SenderForQt.h"

namespace aria2net
{

AriaServer::AriaServer()
    : m_aria2Process(nullptr)
    , m_transceiver(new TransceiverForQt)
{
}

AriaServer::~AriaServer()
{
    std::promise<void> promise;
    m_transceiver->sendTask([this, &promise]() {
        if (m_aria2Process && m_aria2Process->state() == QProcess::Running)
        {
            m_aria2Process->kill();
            m_aria2Process->waitForFinished();
            m_aria2Process.reset();
        }
        promise.set_value();
    });
    promise.get_future().get();
}

void AriaServer::startLocalServerAsync()
{
    m_transceiver->sendTask([&]() {
        m_aria2Process = std::make_unique<QProcess>();
        QString ariaPath = QApplication::applicationDirPath() + "/aria/";
        QString ariaExecutable = QStandardPaths::findExecutable("aria2c", QStringList() << ariaPath);

        QString sessionFile = m_dir + "/aira.session";
        QString logFile = m_dir + "/aira.log";

        if (!QFile::exists(sessionFile))
        {
            QFile file(sessionFile);
            file.open(QFile::WriteOnly);
            file.close();
        }

        if (ariaExecutable.isEmpty())
        {
            ARIA_LOG_INFO("can't find executable file in aria path: ", ariaPath.toStdString());
            if (m_errorFunc)
            {
                m_errorFunc();
            }
        }

        // 设置启动的程序名和命令行参数
        m_aria2Process->setProgram(ariaExecutable);
        m_aria2Process->setArguments(
            QStringList() << "--enable-rpc" << "--rpc-listen-all=true" << "--rpc-allow-origin-all=true" << "--rpc-listen-port=6800" << "--rpc-secret=sVanilla"
                          << "--input-file=" + sessionFile << "--save-session=" + sessionFile << "--save-session-interval=30" << "--log=" + logFile
                          << "--log-level=debug" << "--max-concurrent-downloads=3" << "--max-connection-per-server=16" << "--split=5" << "--min-split-size=10M"
                          << "--max-overall-download-limit=0" << "--max-download-limit=0" << "--max-overall-upload-limit=0" << "--max-upload-limit=0"
                          << "--continue=true" << "--allow-overwrite=true" << "--auto-file-renaming=false" << "--file-allocation=none"
                          << "--header=\"Cookie: \"" << "--header=\"Origin: https://www.bilibili.com\"" << "--header=\"Referer: https://www.bilibili.com\""
                          << "--header=\"User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36 (KHTML, "
                             "like Gecko) Chrome/97.0.4692.99 Safari/537.36\"");

        if (m_aria2Process->state() != QProcess::Running)
        {
            m_aria2Process->start();
        }

        // 检查是否成功启动
        if (!m_aria2Process->waitForStarted())
        {
            ARIA_LOG_INFO(fmt::format("error starting aria2 process: {}", m_aria2Process->errorString().toStdString()));
            if (m_errorFunc)
            {
                m_errorFunc();
            }
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
    m_transceiver->sendTask([this]() {
        if (m_aria2Process && m_aria2Process->state() == QProcess::Running)
        {
            m_aria2Process->kill();
            m_aria2Process->waitForFinished();
            m_aria2Process.reset();
        }
    });
}

void AriaServer::forceCloseServer()
{
    m_transceiver->sendTask([this]() {
        if (m_aria2Process && m_aria2Process->state() == QProcess::Running)
        {
            m_aria2Process->kill();
            m_aria2Process->waitForFinished();
            m_aria2Process.reset();
        }
    });
}

void AriaServer::testServer()
{
    // for getvirsion
}

void AriaServer::setErrorFunc(std::function<void()> func)
{
    m_errorFunc = std::move(func);
}

void AriaServer::setCloseFunc(std::function<void()> func)
{
    m_closeFunc = std::move(func);
}

void AriaServer::setLogDir(const QString& dir)
{
    m_dir = dir + "/aria";
    QDir(m_dir).mkpath(m_dir);
}

}  // namespace aria2net
