#include <QClipboard>
#include <QTimer>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>
#include <QProcess>
#include <QMessageBox>

#include "HomePage.h"
#include "ui_HomePage.h"
#include "BiliApi/BilibiliUrl.h"
#include "ClientUi/Utils/Utility.h"
#include "Plugin/PluginManager.h"
#include "ClientUi/Storage/SearchHistoryStorage.h"
#include "ClientUi//Storage/StorageManager.h"
#include "ClientUi/Login/LoginDialog.h"
#include "Login/BiliLogin.h"
#include "ClientUi/MainWindow/SApplication.h"

inline const std::string mainPage = "https://svanilla.app/";

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

    QProcess process;
    process.setProgram(program);
    process.setArguments(arguments);
    process.start();
    process.waitForFinished();

    if (process.exitStatus() == QProcess::NormalExit && process.exitCode() == 0)
    {
        return true;
    }
    else
    {
        QMessageBox::critical(nullptr, "Error", "Failed to copy file with administrator privileges.");
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

void HomePage::setWebsiteIcon(const QString& iconPath)
{
    ui->lineEditHome->setWebsiteIcon(iconPath);
}

void HomePage::signalsAndSlots()
{
    connect(ui->btnIcon, &QPushButton::clicked, this, [this] {

    });

    connect(ui->lineEditHome, &AddLinkLineEdit::Complete, this, [this] {
        emit parseUri(ui->lineEditHome->text());
        ui->lineEditHome->clear();
    });

    connect(ui->lineEditHome, &AddLinkLineEdit::textChanged, this, [this](const QString& text) {
        if (!text.isEmpty() && text.length() > 1)
        {
            emit updateWebsiteIcon(text.toStdString());
        }
    });

    connect(ui->btnLearn, &QPushButton::clicked, this, [this] {
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
                return;
            }

            plugin::DynamicLibLoader dynamicLibLoader(fileName.toStdString());
            dynamicLibLoader.loadLibrary();
            auto plugin = dynamicLibLoader.loadPluginSymbol();
            if (!plugin)
            {
                return;
            }
        }
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
        std::shared_ptr<AbstractLogin> loginer = std::make_shared<BiliLogin>();
        LoginDialog login(loginer);
        login.exec();
    });

    connect(ui->btnClipBoard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        ui->lineEditHome->setText(clipboard->text());
        emit parseUri(clipboard->text());
        ui->lineEditHome->clear();
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this] {
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
    auto historyStorage = sqlite::StorageManager::intance().searchHistoryStorage();
    auto history = historyStorage->allItems();
    util::createMenu(m_historyMenu, width() / 3, history, actionCallback);
}
