#include <QApplication>
#include <QClipboard>
#include <sstream>
#include <utility>
#include "ClientUi/MainWindow/MainWindow.h"
#include "Util/UrlProcess.h"
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

    SignalsAndSlots();

    startAriaServer();
    QApplication app(_argc, _argv);
    MainWindow maimWindow;
    maimWindow.show();

    return QApplication::exec();
}
void App::loadSettings()
{
    // TODO 读取配置文件失败处理
    m_settings = std::make_shared<Settings>("sVanilla.ini", Settings::IniFormat);

#if 1
    m_settings->write("App", "RPCAddress", "http://10.0.2.122");
    m_settings->write("App", "RPCPort", "6800");
    m_settings->write("App", "TOKEN", "sVanilla");
    m_settings->write("App", "Remote", "true");
#endif

    ariaClient.m_settings = m_settings;
}
void App::setHighDpi()
{
    QApplication::setHighDpiScaleFactorRoundingPolicy(Qt::HighDpiScaleFactorRoundingPolicy::PassThrough);
}
void App::startAriaServer() const
{
    if (!m_settings->read("App", "Remote").toBool())
    {
        aria2net::AriaServer ariaServer;
        qDebug() << "start aria2 local server";
        ariaServer.setErrorFunc([] {});
    }
}

void App::SignalsAndSlots()
{
    connect(Event::getInstance(), &Event::BarBtnClick, this, [this](int index) {
        if (index == 3)
        {
            updateAria2Status();
        }
    });
    // https://filesamples.com/samples/video/mp4/sample_1280x720_surfing_with_audio.mp4
    connect(Event::getInstance(), &Event::ClipboardBtnClick, this, [this]() {
        QClipboard* clipboard = QGuiApplication::clipboard();
        QString originalText = clipboard->text();
//        if (!util::UrlProcess::IsUrl(originalText))
//        {
//            updateErrorMsg("Not a valid url");
//            return;
//        }
        addUri({originalText.toStdString()});
    });
    connect(Event::getInstance(), &Event::AddUri, this, &App::addUri);
}
void App::updateAria2Status()
{
    auto version = std::make_shared<aria2net::AriaVersion>(ariaClient.GetAriaVersionAsync());
    if (!version->id.empty() && (version->error.message.empty())) {
        isConnect = true;
    }
    emit Event::getInstance() -> updateAria2Version(version);
}
void App::updateHomeMsg(std::string msg)
{
    emit Event::getInstance() -> updateMsg(std::move(msg));
}
void App::addUri(const std::list<std::string>& uris)
{
    auto res = ariaClient.AddUriAsync(uris, option, -1);
    if (auto err = res.error.message; !err.empty())
    {
        updateHomeMsg(err);
    } else {
        updateHomeMsg("Add success");
    }
}
