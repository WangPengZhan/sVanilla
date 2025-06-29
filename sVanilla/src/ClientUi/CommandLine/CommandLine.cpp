#include "CommandLine.h"

#include <QCommandLineParser>

#include <iostream>

#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <ftxui/dom/elements.hpp>

#include <indicators/progress_bar.hpp>
#include <indicators/cursor_control.hpp>

#include "MainWindow/SApplication.h"
#include "Config/SingleConfig.h"
#include "version.h"
#include "ClientLog.h"

#ifdef _WIN32
#    include <windows.h>
#    include <fcntl.h>
#    include <io.h>
#endif

void attachConsole()
{
#ifdef _WIN32
    if (AttachConsole(ATTACH_PARENT_PROCESS))
    {
        FILE* fDummy = nullptr;
        freopen_s(&fDummy, "CONOUT$", "w", stdout);
        freopen_s(&fDummy, "CONOUT$", "w", stderr);
        freopen_s(&fDummy, "CONIN$", "r", stdin);
        std::ios::sync_with_stdio();
        static std::shared_ptr<FILE> ptr(fDummy, [](FILE* fDummy) {
            if (fDummy)
            {
                std::cout << std::endl;
                fclose(fDummy);
            }
        });
    }
#endif
}

namespace
{
constexpr char title[] = "Title";
constexpr char publisher[] = "Publisher";
constexpr char cover[] = "Cover";
constexpr char duration[] = "Duration";
constexpr char description[] = "Description";
constexpr char publishDate[] = "PublishDate";
}  // namespace

class CommandLineDownloader : public download::AbstractDownloader
{
public:
    CommandLineDownloader(std::shared_ptr<AbstractDownloader> downloader, indicators::ProgressBar& progressBar)
        : AbstractDownloader()
        , m_realDownloader(downloader)
        , m_progressBar(progressBar)
    {
        setStatus(Waitting);
    }
    ~CommandLineDownloader() = default;

    void setRealDownloader(const std::shared_ptr<AbstractDownloader>& realDownloader)
    {
        m_realDownloader = realDownloader;
    }
    std::shared_ptr<AbstractDownloader> realDownloader()
    {
        return m_realDownloader;
    }

    void start() override
    {
        m_realDownloader->start();
        setStatus(Downloading);
    }
    void stop() override
    {
        m_realDownloader->stop();
        m_progressBar.set_option(indicators::option::PostfixText{"Stopped"});
        setStatus(Stopped);
    }
    void pause() override
    {
        m_realDownloader->pause();
        m_progressBar.set_option(indicators::option::PostfixText{"Paused"});
        setStatus(Paused);
    }
    void resume() override
    {
        m_realDownloader->resume();
        setStatus(Downloading);
    }

    void downloadStatus() override
    {
        m_realDownloader->downloadStatus();
        auto info = m_realDownloader->info();
        m_progressBar.set_progress(static_cast<double>(info.complete) / static_cast<double>(info.total) * 100);
        m_progressBar.set_option(indicators::option::PostfixText{info.stage});
        if (m_realDownloader->status() != Downloading)
        {
            setStatus(m_realDownloader->status());
        }
    }

    void finish() override
    {
        MLogI("CommandLineDownloader", "finish download");
        m_realDownloader->finish();
        setStatus(Finished);
        m_progressBar.set_option(indicators::option::PostfixText{"Finished"});
        m_progressBar.mark_as_completed();
    }

private:
    std::shared_ptr<download::AbstractDownloader> m_realDownloader;
    indicators::ProgressBar& m_progressBar;
};

bool CommandLineOption::isShowHelp() const
{
    return showHelp || url.empty();
}

void CommandLineOption::setHelpText(const std::string& text)
{
    helpText = text;
}

std::string CommandLineOption::getHelpText() const
{
    return helpText;
}

CommandLineOption parseCommandLineOption(int argc, char* argv[])
{
    QStringList args;
    for (int i = 0; i < argc; ++i)
    {
        args << QString::fromLocal8Bit(argv[i]);
    }
    MLogI("CommandLine", "parseCommandLineOption: {}", args.join(" ").toStdString());

    CommandLineOption option;
    if (args.size() == 1)
    {
        option.showGui = true;
        return option;
    }

    QCommandLineParser parser;
    parser.addOption({
        {"h", "help"},
        "Show help"
    });
    parser.addOption({
        {"v", "version"},
        "Show version"
    });
    parser.addOption({
        {"g", "gui"},
        "Show GUI"
    });
    parser.addOption({
        {"d", "dir"},
        "Download directory", "dir"
    });
    parser.addOption({
        {"f", "filename"},
        "Download filename", "filename"
    });
    parser.addOption({
        {"u", "url"},
        "Download url", "url"
    });
    parser.addOption({
        {"c", "cookie"},
        "Cookie", "cookie"
    });
    parser.addOption({
        {"p", "plugin"},
        "Plugin id", "plugin"
    });
    parser.addOption({
        {"sf", "seclect-first"},
        "Select first video view"
    });

    parser.process(args);
    option.setHelpText(parser.helpText().toStdString());

    option.showGui = parser.isSet("gui");
    option.dir = parser.value("dir").toStdString();
    option.filename = parser.value("filename").toStdString();
    option.url = parser.value("url").toStdString();
    option.cookie = parser.value("cookie").toStdString();
    if (parser.isSet("plugin"))
    {
        option.pluginId = parser.value("plugin").toInt();
    }
    option.showHelp = parser.isSet("help");
    option.showVersion = parser.isSet("version");
    option.autoSelectFirst = parser.isSet("seclect-first");

    return option;
}

int execCommandLine(const CommandLineOption& commandLine, SApplication& application)
{
    if (commandLine.showVersion)
    {
        std::cout << "version: " << SVNLA_VERSION_STR_WITH_GIT << std::endl;
        return 0;
    }

    if (commandLine.isShowHelp())
    {
        std::cout << commandLine.getHelpText() << std::endl;
        return 0;
    }

    MLogI("CommandLine", "execCommandLine url: {}", commandLine.url);
    std::cout << "parser url ..." << std::endl;
    std::string localUrl;
    auto plugin = commandLine.pluginId == -1 ? application.pluginInterface().parseUrl(commandLine.url, localUrl) :
                                               application.pluginInterface().getPlugin(commandLine.pluginId);
    if (!plugin)
    {
        MLogE("CommandLine", "parse url failed!");
        std::cerr << "parse url failed" << std::endl;
        return 1;
    }

    if (!commandLine.cookie.empty())
    {
        plugin->loginer().setCookies(commandLine.cookie);
    }

    std::cout << "get video info ..." << std::endl;
    auto views = plugin->getVideoView(localUrl.empty() ? commandLine.url : localUrl);
    if (views.empty())
    {
        MLogE("CommandLine", "get video view failed");
        std::cerr << "get video view failed" << std::endl;
        return 2;
    }

    MLogI("CommandLine", "get video view success, size: {}", views.size());
    adapter::BaseVideoView view;
    if (views.size() == 1 || commandLine.autoSelectFirst)
    {
        view = views.front();
    }
    else
    {
        std::vector<std::vector<std::string>> tableDatas;
        std::vector<std::string> tableHeader = {title, publisher, duration, publishDate, description};
        tableDatas.push_back(tableHeader);
        for (const auto& view : views)
        {
            std::vector<std::string> row;
            for (const auto& header : tableHeader)
            {
                if (header == title)
                {
                    row.push_back(view.Title);
                }
                else if (header == publisher)
                {
                    row.push_back(view.Publisher);
                }
                else if (header == duration)
                {
                    row.push_back(view.Duration);
                }
                else if (header == publishDate)
                {
                    row.push_back(view.PublishDate);
                }
                else if (header == description)
                {
                    row.push_back(view.Description);
                }
                else if (header == cover)
                {
                    row.push_back(view.Cover);
                }
                else
                {
                    row.push_back("");
                }
            }
            tableDatas.push_back(row);
        }

        int selected = 1;

        ftxui::Component tableMenu = ftxui::Renderer([&] {
            ftxui::Elements tableRows;
            for (size_t i = 0; i < tableDatas.size(); ++i)
            {
                const auto& columnData = tableDatas[i];
                bool is_header = i == 0;
                bool is_selected = (int)i == selected;

                std::string marker = is_header ? "   " : (is_selected ? " > " : "   ");
                ftxui::Elements columns;
                ftxui::Color markerColor = is_selected ? ftxui::Color::Green : ftxui::Color::White;
                columns.push_back(ftxui::text(marker) | (is_selected ? ftxui::bold : ftxui::dim) | ftxui::color(markerColor));
                columns.push_back(ftxui::text(columnData[0]) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 80) | ftxui::color(markerColor));
                for (size_t j = 1; j < columnData.size(); ++j)
                {
                    columns.push_back(ftxui::text(" "));
                    columns.push_back(ftxui::text(columnData[j]) | ftxui::size(ftxui::WIDTH, ftxui::EQUAL, 16) | ftxui::color(markerColor));
                }
                auto line = ftxui::hbox(std::move(columns));
                tableRows.push_back(line);
            }

            return ftxui::vbox({ftxui::text("Use ↑ ↓ or Tab to navigate, Enter to select.") | ftxui::bold, ftxui::separator(),
                                ftxui::vbox(std::move(tableRows)) | ftxui::frame});
        });

        auto screen = ftxui::ScreenInteractive::TerminalOutput();
        auto inputHandler = ftxui::CatchEvent(tableMenu, [&](ftxui::Event event) {
            int total_items = tableDatas.size();
            if (event == ftxui::Event::ArrowDown || event == ftxui::Event::Tab)
            {
                selected = selected < total_items - 1 ? selected + 1 : 1;
                return true;
            }
            else if (event == ftxui::Event::ArrowUp)
            {
                selected = selected > 1 ? selected - 1 : total_items - 1;
                return true;
            }
            else if (event == ftxui::Event::Return)
            {
                screen.Exit();
                std::cout << "Selected option: " << selected << tableDatas[selected][0] << std::endl;
                return true;
            }
            return false;
        });

        screen.Loop(inputHandler);
        screen.Clear();
        std::cout << "Selected: " << selected << " - " << tableDatas[selected][0] << std::endl;
        MLogI("CommandLine", "Selected: {} - {}", selected, tableDatas[selected][0]);

        view = views[selected - 1];
    }

    VideoInfoFull info;
    info.videoView = std::make_shared<adapter::BaseVideoView>(view);
    info.downloadConfig = std::make_shared<DownloadConfig>(SingleConfig::instance().downloadConfig());
    if (!commandLine.dir.empty())
    {
        info.downloadConfig->downloadDir = commandLine.dir;
    }
    if (!commandLine.filename.empty())
    {
        info.downloadConfig->nameRule = commandLine.filename;
    }

    auto downloader = plugin->getDownloader(info);
    if (!downloader)
    {
        std::cerr << "get downloader failed" << std::endl;
        return 3;
    }

    indicators::show_console_cursor(false);
    indicators::ProgressBar bar{indicators::option::BarWidth{50},
                                indicators::option::Start{"["},
                                indicators::option::Fill{"="},
                                indicators::option::Lead{">"},
                                indicators::option::Remainder{" "},
                                indicators::option::End{"]"},
                                indicators::option::PostfixText{"Waiting"},
                                indicators::option::ForegroundColor{indicators::Color::green},
                                indicators::option::ShowPercentage{true},
                                indicators::option::FontStyles{std::vector<indicators::FontStyle>{indicators::FontStyle::bold}}};
    bar.set_progress(0.0);
    auto commandLineDownloader = std::make_shared<CommandLineDownloader>(downloader, bar);
    application.downloadThread().addTaks(commandLineDownloader);
    commandLineDownloader->setStatus(download::AbstractDownloader::Ready);

    MLogI("CommandLine", "Waitting downloadfinished!");
    while (true)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        if ((commandLineDownloader->status() == download::AbstractDownloader::Finished ||
             commandLineDownloader->status() == download::AbstractDownloader::Error) &&
            application.downloadThread().empty())
        {
            break;
        }
    }

    indicators::show_console_cursor(true);

    if (commandLineDownloader->status() == download::AbstractDownloader::Error)
    {
        std::cerr << "download failed" << std::endl;
        MLogI("CommandLine", "Download error! url: {}", commandLine.url);
        return 4;
    }

    return 0;
}
