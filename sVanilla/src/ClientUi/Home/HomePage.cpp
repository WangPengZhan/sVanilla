#include <QClipboard>
#include <QTimer>
#include <QMenu>
#include <QPushButton>
#include <QStandardPaths>
#include <QDesktopServices>
#include <QFileDialog>

#include "HomePage.h"
#include "ui_HomePage.h"
#include "BiliApi/BilibiliUrl.h"
#include "Plugin/PluginManager.h"
#include "Util/HistoryUtil.h"

inline const std::string mainPage = "https://svanilla.app/";

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

    connect(ui->lineEditHome, &SearchLineEdit::Complete, this, [this] {
        emit parseUri({ui->lineEditHome->text().toStdString()});
    });

    connect(ui->lineEditHome, &SearchLineEdit::textChanged, this, [this](const QString& text) {
        if (!text.isEmpty() && text.length() > 1)
        {
            emit updateWebsiteIcon(text.toStdString());
        }
    });

    connect(ui->btnSubmit, &QPushButton::clicked, this, [this] {
        emit parseUri({ui->lineEditHome->text().toStdString()});
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

        QFile::copy(fileName, newPlugin);
    });
    connect(ui->btnLoginWebsite, &QPushButton::clicked, this, [this] {

    });

    connect(ui->btnClipBoard, &QPushButton::clicked, this, [this] {
        const QClipboard* clipboard = QGuiApplication::clipboard();
        emit parseUri({clipboard->text().toStdString()});
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

    const auto maxWidth = width() / 3;
    for (const auto& uri : SearchHistory::global().history())
    {
        const auto text = QString::fromStdString(uri);
        QString elidedText = text;
        if (const QFontMetrics fontMetrics(m_historyMenu->font()); fontMetrics.horizontalAdvance(text) > maxWidth)
        {
            elidedText = fontMetrics.elidedText(text, Qt::ElideRight, maxWidth);
        }

        auto* const action = m_historyMenu->addAction(elidedText, this, [this, uri] {
            ui->lineEditHome->setText(QString::fromStdString(uri));
        });
        action->setToolTip(text);
    }
}
