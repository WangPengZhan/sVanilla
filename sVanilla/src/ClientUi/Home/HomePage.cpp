#include <QClipboard>
#include <QTimer>
#include <QDebug>
#include <QPushButton>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>

#include "HomePage.h"
#include "ui_HomePage.h"
#include "Plugin/PluginManager.h"
#include "Util/UrlProcess.h"
#include "VanillaStyle/Style.h"

constexpr char mainPage[] = "https://svanilla.app/";

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

void HomePage::signalsAndSlots()
{
    connect(ui->btnIcon, &QPushButton::clicked, this, [this] {

    });

    connect(ui->lineEditHome, &SearchLineEdit::Complete, this, [this] {
        qDebug() << ui->lineEditHome->text();
        parseUri({ui->lineEditHome->text().toStdString()});
    });

    connect(ui->btnLearn, &QPushButton::clicked, this, [this] {
        QDesktopServices::openUrl(QUrl(mainPage));
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

        QFile::copy(fileName, newPlugin);
    });
    connect(ui->btnLoginWebsite, &QPushButton::clicked, this, [this] {

    });

    connect(ui->btnClipBoard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        if (const QString originalText = clipboard->text(); !util::UrlProcess::IsUrl(originalText))
        {
        }
        else
        {
            parseUri({originalText.toStdString()});
        }
    });
    connect(ui->btnHistory, &QPushButton::clicked, this, [this] {

    });
}

void HomePage::setUi()
{
    constexpr int iconSize = 40;
    ui->btnIcon->setIconSize({iconSize, iconSize});

    constexpr int homeLineEditHeight = 30;
    ui->lineEditHome->setFixedHeight(homeLineEditHeight);
}

void HomePage::parseUri(const std::string& uri)
{
    // if bili
    emit loadBiliViewView(uri);
}
