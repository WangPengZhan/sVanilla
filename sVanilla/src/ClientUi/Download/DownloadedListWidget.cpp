#include <QProcess>
#include <QDir>
#include <QPushButton>

#include <utility>

#include "DownloadedListWidget.h"
#include "ui_DownloadedListWidget.h"
#include "ClientUi/VideoList/VideoData.h"
#include "Adapter/BaseVideoView.h"

DownloadedItemWidget::DownloadedItemWidget(std::shared_ptr<VideoInfoFull> videoInfoFull, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadedItemWidget)
    , m_listWidget(nullptr)
    , m_videoInfoFull(std::move(videoInfoFull))
{
    ui->setupUi(this);
    signalsAndSlots();
    ui->labelTitle->setText(QString::fromStdString(m_videoInfoFull->videoView->Title));
    ui->btnRestart->setIcon(QIcon(":icon/download/start.svg"));
    ui->btnDelete->setIcon(QIcon(":icon/download/delete.svg"));
    ui->btnFolder->setIcon(QIcon(":icon/download/folder.svg"));
    ui->btnDetail->setIcon(QIcon(":icon/download/details.svg"));
    setBackgroundRole(QPalette::NoRole);
}

DownloadedItemWidget::~DownloadedItemWidget()
{
    delete ui;
}

void DownloadedItemWidget::setListWidget(DownloadedListWidget* listWidget)
{
    m_listWidget = listWidget;
}

DownloadedListWidget* DownloadedItemWidget::listWidget() const
{
    return m_listWidget;
}

std::shared_ptr<VideoInfoFull> DownloadedItemWidget::videoInfoFull() const
{
    return m_videoInfoFull;
}

void DownloadedItemWidget::setStart()
{
}

void DownloadedItemWidget::setStop()
{
}

void DownloadedItemWidget::signalsAndSlots()
{
    connect(ui->btnDelete, &QPushButton::clicked, this, [this]() {
        if (auto item = m_listWidget->itemFromWidget(this))
        {
            delete item;
            deleteLater();
            return;
        }
    });
    connect(ui->btnRestart, &QPushButton::clicked, this, [this](bool isResume) {

    });
    connect(ui->btnFolder, &QPushButton::clicked, this, [this]() {
        QString filePath = QString::fromStdString("test.mp4");
        if (std::filesystem::u8path("test.mp4").is_relative())
        {
            filePath = QApplication::applicationDirPath() + "/" + filePath;
        }

        QStringList arguments;

#ifdef _WIN32
        QString explorerCommand = "explorer";
        arguments << "/select," << QDir::toNativeSeparators(filePath);
#elif __linux__
        QString explorerCommand = "open";
        arguments << "-R" << filePath;
#elif __APPLE__
        QString explorerCommand = "open";
        arguments << QStringLiteral("-R") << "\"" << filePath << "\"";
#endif

        QProcess::startDetached(explorerCommand, arguments);
    });
}

DownloadedListWidget::DownloadedListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadedListWidget"));
    signalsAndSlots();
    setBackgroundRole(QPalette::NoRole);
}

void DownloadedListWidget::addDownloadedItem(const std::shared_ptr<VideoInfoFull>& videoInfFull)
{
    auto pWidget = new DownloadedItemWidget(videoInfFull, this);
    pWidget->setListWidget(this);
    auto pItem = new QListWidgetItem(this);
    pItem->setSizeHint(pWidget->sizeHint());
    setItemWidget(pItem, pWidget);
    m_items.insert({videoInfFull->getGuid(), pItem});
}

void DownloadedListWidget::startAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (downloadItemWidget(i))
        {
            downloadItemWidget(i)->setStart();
        }
    }
}

void DownloadedListWidget::stopAll()
{
    int nCount = count();
    for (int i = 0; i < nCount; ++i)
    {
        if (downloadItemWidget(i))
        {
            downloadItemWidget(i)->setStop();
        }
    }
}

void DownloadedListWidget::deleteAll()
{
    m_items.clear();
    clear();
}

void DownloadedListWidget::removeDownloadItem(const std::string& guid)
{
    if (m_items.find(guid) != m_items.end())
    {
        if (m_items[guid])
        {
            delete m_items[guid];
        }
        m_items.erase(guid);
    }
}

QListWidgetItem* DownloadedListWidget::itemFromWidget(QWidget* target)
{
    for (int i = 0; i < count(); ++i)
    {
        if (itemWidget(item(i)) == target)
        {
            return item(i);
        }
    }

    return nullptr;
}

DownloadedItemWidget* DownloadedListWidget::downloadItemWidget(int row) const
{
    return qobject_cast<DownloadedItemWidget*>(itemWidget(item(row)));
}

void DownloadedListWidget::signalsAndSlots() const
{
}
