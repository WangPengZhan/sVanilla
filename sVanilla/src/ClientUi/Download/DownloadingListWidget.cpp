#include "DownloadingListWidget.h"

#include <utility>
#include "ui_DownloadingListWidget.h"
#include "Theme/StyledItemDelegate.h"

template <typename T>
QString formatSize(T bytesPerSec)
{
    const double Gib = pow(2, 30);
    const double Mib = pow(2, 20);
    const double Kib = pow(2, 10);
    if (bytesPerSec >= Gib)
    {
        return QString::number(bytesPerSec / Gib, 'g', 2) + "GiB/s";
    }
    if (bytesPerSec >= Mib)
    {
        return QString::number(bytesPerSec / Mib, 'g', 2) + "Mib/s";
    }
    if (bytesPerSec >= Kib)
    {
        return QString::number(bytesPerSec / Kib, 'g', 2) + "Kib/s";
    }
    return QString::number(bytesPerSec, 'g', 3) + "B/s";
}

DownloadingItemWidget::DownloadingItemWidget(std::string gid, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::DownloadingItemWidget)
    , m_gid(std::move(gid))
{
    ui->setupUi(this);
    signalsAndSlots();
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::signalsAndSlots()
{
    connect(ui->Delete, &QPushButton::clicked, this, [this] {
        emit deleteBtnClick(m_gid);
    });
}
void DownloadingItemWidget::updateStatus()
{
    const std::filesystem::path path(status->result.files.front().path);
    ui->Title->setText(QString::fromStdString(path.stem().string()));
    ui->Speed->setText(formatSize(QString::fromStdString(status->result.downloadSpeed).toInt()));
    const auto progress = QString::fromStdString(status->result.completedLength).toInt() * 100 / QString::fromStdString(status->result.totalLength).toInt();
    ui->progressBar->setValue(progress);
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    signalsAndSlots();
    auto* delegate = new CustomVideoListItemDelegate();
    this->setItemDelegate(delegate);
    addTaskItem("1234");
    addTaskItem("1235");
    addTaskItem("1236");

}
void DownloadingListWidget::signalsAndSlots() const
{

}

void DownloadingListWidget::addTaskItem(const std::string& gid)
{
    const auto newItem = new DownloadingItemWidget(gid, this);
    const auto listWidgetItem = new QListWidgetItem(this);
    listWidgetItem->setSizeHint(newItem->sizeHint());
    this->setItemWidget(listWidgetItem, newItem);
    m_items.insert(std::make_pair(gid, listWidgetItem));
    connect(newItem, &DownloadingItemWidget::deleteBtnClick, this, &DownloadingListWidget::deleteItem);
}

void DownloadingListWidget::updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    const auto gid = status->result.gid;
    const auto item = itemWidget(m_items[gid]);
    const auto widget = qobject_cast<DownloadingItemWidget*>(item);
    widget->status = status;
    widget->updateStatus();
}

void DownloadingListWidget::deleteItem(const std::string& gid)
{
    const int row = this->row(m_items[gid]);
    this->takeItem(row);
    this->update();
    m_items.erase(gid);
}
