#include "DownloadingListWidget.h"

#include <utility>
#include "ui_DownloadingListWidget.h"
#include "ClientUi/Event.h"

DownloadingItemWidget::DownloadingItemWidget(std::string gid, QWidget* parent)
    : QWidget(parent),
      ui(new Ui::DownloadingItemWidget),
      gid(std::move(gid))
{
    ui->setupUi(this);
    // this->setStyleSheet("    background-color: #000000; border-radius: 5px;");
}

DownloadingItemWidget::~DownloadingItemWidget()
{
    delete ui;
}

void DownloadingItemWidget::SetUi()
{
    // ui->labelTitle->setText(QString::fromStdString(name));
    ui->Title->setText(QString::fromStdString(name));
    ui->Size->setText(QString::fromStdString(size));
}

void DownloadingItemWidget::SignalsAndSlots()
{
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent), downloadIntervalTimer(new QTimer(this))
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    SignalsAndSlots();
}
void DownloadingListWidget::SignalsAndSlots() const
{
    //  addTaskItem signal -> addTaskItem (core -> ui)
    connect(Event::getInstance(), &Event::AddDownloadTask, this, &DownloadingListWidget::addTaskItem);
    // update download information to ui (core -> ui)
    connect(Event::getInstance(), &Event::updateDownloadStatus, this, &DownloadingListWidget::updateItem);
    // interval update download status (timer -> core)
    connect(downloadIntervalTimer, &QTimer::timeout,Event::getInstance(), &Event::IntervalUpdateDownloadStatus);
    // current row changed signal -> onCurrent (ui -> core)
    connect(Event::getInstance(), &Event::OnDownloadCurrent, this, &DownloadingListWidget::onCurrent);
}

void DownloadingListWidget::onCurrent(bool isCurrent)
{
    if (isCurrent)
    {
        downloadIntervalTimer->start(1000);
        isTiemrStart = true;
    }
    else if (isTiemrStart)
    {
        downloadIntervalTimer->stop();
        isTiemrStart = false;
    }
}
void DownloadingListWidget::addTaskItem(const std::string& gid)
{
    const auto newItem = new DownloadingItemWidget(gid, this);
    const auto item = qobject_cast<QWidget*>(newItem);
    const auto itemWidget = new QListWidgetItem(this);
    itemWidget->setSizeHint(newItem->sizeHint());
    this->setItemWidget(itemWidget, item);
    _items.insert({gid, newItem});
}
void DownloadingListWidget::updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    const auto item = _items[status->result.gid];
    if (!status->result.errorCode.empty())
    {
        item->status = 1;
        item->name = status->result.files.front().path;
        item->size = status->result.totalLength;
        item->speed = status->result.downloadSpeed;
    }
    else
    {
        item->status = 0;
    }
    item->update();
}
