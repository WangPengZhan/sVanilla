#include <QApplication>
#include <sstream>
#include <utility>
#include "ClientUi/MainWindow/MainWindow.h"
#include "Logger/Dump.h"
#include "Logger/Logger.h"

#include "App.h"
#include "ClientUi/Event.h"

App::App(int argc, char** argv)
    : _argc(argc),
      _argv(argv)
{
}

int App::exec()
{
    DumpColletor::registerDumpHandle();
    Logger::getInstance();

    setHighDpi();
    loadSettings();
    downloadManager = std::make_shared<DownloadManager>();

    QApplication app(_argc, _argv);
    MainWindow maimWindow;
    maimWindow.show();

    SignalsAndSlots();
    startAriaServer();
    return QApplication::exec();
}
void App::loadSettings()
{
    // TODO 读取配置文件失败处理
    settings = std::make_shared<Settings>("sVanilla.ini", Settings::IniFormat);

#if 1
    settings->write("App", "RPCAddress", "http://10.0.2.122");
    settings->write("App", "RPCPort", "6800");
    settings->write("App", "TOKEN", "sVanilla");
    settings->write("App", "Remote", "true");
#endif

#if 0
    settings->write("App", "RPCAddress", "http://localhost");
    settings->write("App", "RPCPort", "6800");
    settings->write("App", "TOKEN", "sVanilla");
    settings->write("App", "Remote", "false");
#endif

    ariaClient.m_settings = settings;
}
void App::setHighDpi()
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}
void App::startAriaServer() const
{
    if (!settings->read("App", "Remote").toBool())
    {
        aria2net::AriaServer ariaServer;
        PRINT("start aria2 local server")
        ariaServer.setErrorFunc([] {});
    }
}

void App::SignalsAndSlots()
{
    // window bar button clicked signal -> Event::BarBtnClick (ui -> core)
    connect(Event::getInstance(), &Event::BarBtnClick, this, [this](int index) {
        if (index == 2)
        {
            emit Event::getInstance()->OnDownloadCurrent(true);
        }
        else
        {
            emit Event::getInstance()->OnDownloadCurrent(false);
        }
        if (index == 3)
        {
            updateAria2Status();
        }
    });

    // AddUri signal -> Event::AddUri (ui -> core)
    connect(Event::getInstance(), &Event::AddUri, this, &App::addUri);

    // download interval timer -> Event::IntervalUpdateDownloadStatus (ui -> core)
    connect(Event::getInstance(), &Event::IntervalUpdateDownloadStatus, this, &App::updateDownloadStatus);
}

void App::updateAria2Status()
{
    const auto version = std::make_shared<aria2net::AriaVersion>(ariaClient.GetAriaVersionAsync());
    if (!version->id.empty() && (version->error.message.empty()))
    {
        isConnect = true;
    }
    emit Event::getInstance()->updateAria2Version(version);
}

void App::updateDownloadStatus()
{
    const std::list<std::string>& gids = downloadManager->downloadGIDs;
    if (gids.empty())
    {
        return;
    }
    for (const auto& gid : gids)
    {
        const auto status = std::make_shared<aria2net::AriaTellStatus>(ariaClient.TellStatus(gid));
        if (!status->result.gid.empty() && status->error.message.empty())
        {
            emit Event::getInstance()->updateDownloadStatus(status);
        }
    }
}

void App::addUri(const std::list<std::string>& uris)
{
    const auto res = ariaClient.AddUriAsync(uris, option, -1);
    if (const auto err = res.error.message; !err.empty())
    {
        updateHomeMsg(err);
    }
    else
    {
        downloadManager->addDownloadGID(res.result);
        updateHomeMsg("Add success");
    }
}

void App::updateHomeMsg(std::string msg)
{
    emit Event::getInstance()->updateMsg(std::move(msg));
}
