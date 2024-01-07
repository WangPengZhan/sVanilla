#include <QApplication>
#include <QClipboard>
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

    SignalsAndSlots();

    startAriaServer();
//    startAriaClient();
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
    if (!m_settings->read("App", "Remote").toBool()) {
        aria2net::AriaServer ariaServer;
        ariaServer.setErrorFunc([] {});
    }
}

void App::SignalsAndSlots()
{
    connect(Event::getInstance(), &Event::BarBtnClick, this, [this](int index) {
        if (index == 4) {
            updateAria2Status();
        }
    });
    //https://filesamples.com/samples/video/mp4/sample_1280x720_surfing_with_audio.mp4
    connect(Event::getInstance(), &Event::ClipboardBtnClick, this, [this]() {
        qDebug() << "SearchBtnClick";
        QClipboard *clipboard = QGuiApplication::clipboard();
        QString originalText = clipboard->text();
        auto res = ariaClient.AddUriAsync({originalText.toStdString()}, option,0);
        qDebug() << res.result;
        qDebug() << res.error.message;
    });
    connect(Event::getInstance(), &Event::addSingleUri, this, [this](std::string uri) {
        std::list<std::string> uris = {std::move(uri)};
        auto res = ariaClient.AddUriAsync(uris, option,-1);
        qDebug() << res.result.c_str();
        qDebug() << res.error.message;
    }
    );
}
void App::updateAria2Status()
{
    auto status = ariaClient.GetAriaVersionAsync();
    if (status.id.empty()) {
        emit Event::getInstance()->updateAria2Status("Disconnected");
        return;
    }
    emit Event::getInstance()->updateAria2Status("Connected");
    std::string version = status.result.version;
    emit Event::getInstance()->updateVersion(std::move(version));

    std::list<std::string> features = status.result.enabledFeatures;
    std::stringstream ss;
    for(const auto& str : features) {
        ss << str << "\n";
    }
    emit Event::getInstance()->updateFeatures(std::move(ss.str()));
}
