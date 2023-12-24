#include <QApplication>
// #include <QFile>
#include <QDir>
// #include <QDebug>
#include <QProcess>
// #include <chrono>
// #include <thread>
// #include <future>
#include <memory>
// #include <windows.h>
#include <QStandardPaths>

#include "AriaServer.h"
#include "Aria2Net/AriaLog.h"

namespace aria2net
{

AriaServer::AriaServer(QObject* parent) : QObject(parent), aria2Process(std::make_unique<QProcess>(this))
{
}

AriaServer::~AriaServer()
{
    CloseServer();
}

void AriaServer::StartServerAsync()
{

    QString ariaPath = QApplication::applicationDirPath() + "/aria/";
    QString ariaExecutable = QStandardPaths::findExecutable("aria2c", QStringList() << ariaPath);
    QString sessionFile = ariaPath + "aira.session";
    QString logFile = ariaPath + "log.txt";

    QDir airDir = QDir(ariaPath);

    if (!ariaExecutable.isEmpty())
    {
        bool ok = airDir.mkpath(ariaPath);
        if (!ok)
        {
//            return ok;
        }
    }

    if (!QFile::exists(sessionFile))
    {
        QFile file(sessionFile);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }

    if (!QFile::exists(logFile))
    {
        QFile file(logFile);
        file.open(QIODevice::ReadWrite | QIODevice::Text);
        file.close();
    }

    aria2Process->setProgram(ariaExecutable);
    aria2Process->setArguments(QStringList() << "--enable-rpc"
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

    // 设置启动的程序名和命令行参数
    if (aria2Process->state() != QProcess::Running)
    {
        aria2Process->start();
    }

    // 检查是否成功启动
    if (!aria2Process->waitForStarted())
    {
        //            qDebug() << "Error starting aria2 process:" << aria2Process->errorString();
        ARIA_LOG_INFO({aria2Process->errorString().toStdString()},"Error starting aria2 process: {1}");
    }
    else
    {
        ARIA_LOG_INFO("Aria2 process started successfully.");
    }
}

void AriaServer::CloseServer()
{
    if (aria2Process && aria2Process->state() == QProcess::Running)
    {
        aria2Process->kill();
        aria2Process->waitForFinished();
    }
}


}  // namespace aria2net
