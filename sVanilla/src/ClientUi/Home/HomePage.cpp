#include <QClipboard>
#include <QTimer>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>
#include <QProcess>

#include "HomePage.h"
#include "ui_HomePage.h"
#include "BaseQt/Utility.h"
#include "Plugin/PluginManager.h"
#include "Storage/SearchHistoryStorage.h"
#include "Storage/StorageManager.h"
#include "Login/LoginDialog.h"
#include "Login/LoginWebDialog.h"
#include "Login/SetCookieDialog.h"
#include "MainWindow/SApplication.h"
#include "ClientLog.h"
#include "const_string.h"
#include "Login/LoginBubble.h"
#include "Utils/RunTask.h"
#include "SUI/Tips/Toast.h"
#include "SUI/Tips/ToastTip.h"
#include "SUI/Tips/TipDialog.h"

inline const std::string mainPage = "https://svanilla.app/";
constexpr char userfaceDir[] = "userface";

bool copyWithAdminPrivileges(const QString& source, const QString& destination)
{
    QStringList arguments;
    QString program;

#ifdef _WIN32
    program = "powershell";
    QString command = QString("Copy-Item -Path '%1' -Destination '%2' -Force").arg(source).arg(destination);
    arguments << QString("Start-Process powershell -ArgumentList '-NoProfile', '-ExecutionPolicy Bypass', '-Command', \"%1\" -Verb RunAs").arg(command);
#elif __linux__
    program = "sudo";
    arguments << "-S" << QString("cp \"%1\" \"%2\"").arg(source).arg(destination);
#elif __APPLE__
    program = "osascript";
    arguments << "-e" << QString("do shell script \"cp '%1' '%2'\" with administrator privileges").arg(source).arg(destination);
#endif

    MLogI(svanilla::cHomeModule, "copyWithAdmin started! command: {}, arguments: {}", program.toStdString(), arguments.join(" ").toStdString());

    CLog_Unique_TimerK(copyWithAdmin_process_start);
    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();
    CLog_Unique_TimerK_END(copyWithAdmin_process_start);

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0)
    {
        return true;
    }
    else
    {
        MLogE(svanilla::cHomeModule, "Failed to copy file with administrator privileges, source: {}, destination: {}", source.toStdString(),
              destination.toStdString());
        TipDialog dialog;
        dialog.showTip(QObject::tr("Failed to copy file without administrator privileges."), TipDialog::Error);
        dialog.setNoCancelButton(true);
        dialog.exec();
        return false;
    }
}

HomePage::HomePage(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::HomePage)
{
    ui->setupUi(this);
    signalsAndSlots();
    setUi();
}

HomePage::~HomePage()
{
    delete ui;
}

void HomePage::setWebsiteIcon(const QIcon& icon)
{
    ui->lineEditHome->setWebsiteIcon(icon);
}

void HomePage::signalsAndSlots()
{
    connect(ui->btnIcon, &QPushButton::clicked, this, [this] {});

    connect(ui->lineEditHome, &AddLinkLineEdit::Complete, this, [this] {
        if (ui->lineEditHome->text().isEmpty())
        {
            return;
        }

        emit parseUri(ui->lineEditHome->text());
        ui->lineEditHome->clear();
        ui->lineEditHome->setWebsiteIcon(QIcon(":/icon/web_default_icon.svg"));
    });

    connect(ui->lineEditHome, &AddLinkLineEdit::textChanged, this, [this](const QString& text) {
        if (text.isEmpty())
        {
            ui->lineEditHome->setWebsiteIcon(QIcon(":/icon/web_default_icon.svg"));
            return;
        }

        if (sApp->isLoadingPlugins())
        {
            MLogI(svanilla::cHomeModule, "plugins are loading, skip url icon parsing: {}", text.toStdString());
            ui->lineEditHome->setWebsiteIcon(QIcon(":/icon/web_default_icon.svg"));
            return;
        }

        static uint64_t getPluginVersion = 0;
        auto taskFunc = [text]() {
            std::string locationUrl;
            return sApp->pluginInterface().parseUrl(text.toStdString(), locationUrl);
        };

        getPluginVersion++;
        uint64_t capturedVersion = getPluginVersion;
        auto callback = [this, capturedVersion, text](std::shared_ptr<plugin::IPlugin> plugin) {
            if (capturedVersion != getPluginVersion)
            {
                MLogI(svanilla::cVideoList, "return version different: capture-{}, now-{}", capturedVersion, getPluginVersion);
                return;
            }

            if (!plugin)
            {
                MLogI(svanilla::cVideoList, "can't parser url: {}", text.toStdString());
                ui->lineEditHome->setWebsiteIcon(QIcon(":/icon/web_default_icon.svg"));
                return;
            }

            constexpr QSize iconSize(24, 24);
            QIcon icon(util::binToImage(plugin->websiteIcon(), iconSize * sApp->devicePixelRatio()));
            ui->lineEditHome->setWebsiteIcon(icon);
        };

        runTask(taskFunc, callback, this);
    });

    connect(ui->btnLearn, &QPushButton::clicked, this, [this] {
        MLogI(svanilla::cHomeModule, "btnLearn clicked");
        QDesktopServices::openUrl(QUrl(QString::fromStdString(mainPage)));
    });
    connect(ui->btnLoadPlugin, &QPushButton::clicked, this, [this] {
        const QString pluginDir = QDir(QString::fromStdString(plugin::PluginManager::m_pluginDir)).absolutePath();
        const QString fileName =
            QFileDialog::getOpenFileName(this, tr("Import Plugin"), {}, QString("*") + QString::fromStdString(plugin::PluginManager::m_dynamicExtension));
        QString newPlugin = pluginDir + "/" + QFileInfo(fileName).fileName();
        {
            if (QFile::exists(newPlugin))
            {
                MLogW(svanilla::cHomeModule, "import existed plugin, filename: {}", fileName.toStdString());
                return;
            }

            plugin::DynamicLibLoader dynamicLibLoader(fileName.toStdString());
            dynamicLibLoader.loadLibrary();
            auto plugin = dynamicLibLoader.loadPluginSymbol();
            if (!plugin)
            {
                MLogW(svanilla::cHomeModule, "load plugin filed, filename: {}", fileName.toStdString());
                return;
            }
        }

        MLogI(svanilla::cHomeModule, " import plugin, source: {},destination: {} ", fileName.toStdString(), newPlugin.toStdString());
        if (sApp->isInstalled())
        {
            copyWithAdminPrivileges(fileName, newPlugin);
        }
        else
        {
            QFile::copy(fileName, newPlugin);
        }
    });
    connect(ui->btnLoginWebsite, &QPushButton::clicked, this, [this] {
        if (sApp->isLoadingPlugins())
        {
            MLogW(svanilla::cHomeModule, "plugins are loading, login plugin list is not ready");
            ToastTip::showTip(tr("Plugins are loading, please try again later"), ToastTip::Warn);
            return;
        }

        auto plugins = sApp->pluginManager().pluginsSnapshot();
        std::vector<std::shared_ptr<plugin::IPlugin>> loginPlugins;
        for (auto& plugin : plugins)
        {
            if (plugin->loginer().supportsLogin())
            {
                loginPlugins.push_back(plugin);
            }
        }

        if (loginPlugins.size() >= 1)
        {
            QMenu menu(this);
            bool hasNoLogined = false;
            for (auto& plugin : loginPlugins)
            {
                if (!plugin->loginer().isLoggedIn())
                {
                    hasNoLogined = true;
                }

                auto action = new QAction(QString::fromStdString(plugin->pluginMessage().name), &menu);
                constexpr QSize iconSize(24, 24);
                QIcon icon(util::binToImage(plugin->websiteIcon(), iconSize * sApp->devicePixelRatio()));
                action->setIcon(icon);
                menu.addAction(action);
                connect(action, &QAction::triggered, &menu, [this, plugin]() {
                    auto loginer = std::make_shared<LoginProxy>(plugin->loginer());
                    showLoginDialog(loginer);
                });
            }

#ifdef _DEBUG
            if (true)
#else
            if (hasNoLogined)
#endif
            {
                menu.addSeparator();
                auto addCookieAction = new QAction(tr("Set Cookie"), &menu);
                QIcon icon(":/icon/cookie.svg");
                addCookieAction->setIcon(icon);
                menu.addAction(addCookieAction);
                connect(addCookieAction, &QAction::triggered, &menu, [this, loginPlugins]() {
                    SetCookieDialog dialog(loginPlugins, this);
                    if (QDialog::Accepted == dialog.exec())
                    {
                        MLogI(svanilla::cHomeModule, " LoginWebsite succeed");
                        emit switchAccoutTab();
                        emit loginSucceed(dialog.loginer());
                    }
                });
            }
            const QPoint pos = ui->btnLoginWebsite->mapToGlobal(QPoint(0, ui->btnLoginWebsite->sizeHint().height()));
            menu.exec(pos);
        }
        else
        {
            MLogW(svanilla::cHomeModule, "No login plugin available");
            ToastTip::showTip(tr("No login plugin available"), ToastTip::Warn);
        }
    });

    connect(ui->btnClipBoard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        MLogI(svanilla::cHomeModule, " btnClipBoard, search: {}", clipboard->text().toStdString());
        emit parseUri(clipboard->text());
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this] {
        MLogI(svanilla::cHomeModule, " btnHistory clicked");
        createHistoryMenu();
        const QPoint pos = ui->btnHistory->mapToGlobal(QPoint(0, -m_historyMenu->sizeHint().height()));
        m_historyMenu->exec(pos);
    });
}

void HomePage::setUi()
{
    constexpr int iconSize = 40;
    ui->btnIcon->setIconSize({iconSize, iconSize});
    constexpr int homeLineEditHeight = 30;
    ui->lineEditHome->setFixedHeight(homeLineEditHeight);
    ui->lineEditHome->setWebsiteIcon(QIcon(":/icon/web_default_icon.svg"));
}

void HomePage::createHistoryMenu()
{
    if (m_historyMenu == nullptr)
    {
        m_historyMenu = new QMenu(this);
    }
    else
    {
        m_historyMenu->clear();
    }

    const auto actionCallback = [this](const QString& text) {
        ui->lineEditHome->setText(text);
        ui->lineEditHome->setFocus();
    };
    auto historyStorage = sqlite::StorageManager::instance().searchHistoryStorage();
    auto history = historyStorage->allItems();
    util::createMenu(m_historyMenu, width() / 3, history, actionCallback);
}

void HomePage::showLoginDialog(std::shared_ptr<LoginProxy> loginer)
{
    if (!loginer)
    {
        return;
    }

    if (loginer->isLoggedIn())
    {
        emit switchAccoutTab();
    }
    else
    {
        MLogI(svanilla::cHomeModule, " LoginWebsite ");
        if (loginer->loginWay() == AbstractLoginApi::Web)
        {
            LoginWebDialog login(loginer);
            if (QDialog::Accepted == login.exec())
            {
                MLogI(svanilla::cHomeModule, " LoginWebsite succeed");
                emit switchAccoutTab();
                emit loginSucceed(loginer);
            }
        }
        else
        {
            LoginDialog login(loginer, this);
            if (QDialog::Accepted == login.exec())
            {
                MLogI(svanilla::cHomeModule, " LoginWebsite succeed");
                emit switchAccoutTab();
                emit loginSucceed(loginer);
            }

            // const auto loginBubble = new LoginBubble(loginer);
            // const auto globalPos = mapToGlobal(QPoint(0, 0));
            // loginBubble->showCenter(QRect(globalPos, QSize(width(), height())));
        }
    }
}
