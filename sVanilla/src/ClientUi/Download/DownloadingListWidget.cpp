#include "DownloadingListWidget.h"

#include <utility>
#include "ui_DownloadingListWidget.h"

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
    ui->Title->setText(QString::fromStdString(status->result.dir));
}

DownloadingListWidget::DownloadingListWidget(QWidget* parent)
    : QListWidget(parent)
{
    this->setObjectName(QStringLiteral("DownloadingListWidget"));
    signalsAndSlots();
    addTaskItem("123");
    addTaskItem("456");
    addTaskItem("789");
}
void DownloadingListWidget::signalsAndSlots() const
{
    // //  addTaskItem signal -> addTaskItem (core -> ui)
    // connect(Event::getInstance(), &Event::AddDownloadTask, this, &DownloadingListWidget::addTaskItem);
    // // update download information to ui (core -> ui)
    // connect(Event::getInstance(), &Event::updateDownloadStatus, this, &DownloadingListWidget::updateItem);
    // // interval update download status (timer -> core)
    // connect(downloadIntervalTimer, &QTimer::timeout, Event::getInstance(), &Event::IntervalUpdateDownloadStatus);
    // // current row changed signal -> onCurrent (ui -> core)
    // connect(Event::getInstance(), &Event::OnDownloadCurrent, this, &DownloadingListWidget::onCurrent);
}
//
// void DownloadingListWidget::onCurrent(const bool isCurrent)
// {
//     if (isCurrent)
//     {
//         downloadIntervalTimer->start(1000);
//         isTiemrStart = true;
//     }
//     else if (isTiemrStart)
//     {
//         downloadIntervalTimer->stop();
//         isTiemrStart = false;
//     }
// }
void DownloadingListWidget::addTaskItem(const std::string& gid)
{
    const auto newItem = new DownloadingItemWidget(gid, this);
    const auto listWidgetItem = new QListWidgetItem(this);
    listWidgetItem->setSizeHint(newItem->sizeHint());
    this->setItemWidget(listWidgetItem, newItem);
    m_items.insert({gid, listWidgetItem});
    connect(newItem, &DownloadingItemWidget::deleteBtnClick, this, &DownloadingListWidget::deleteItem);
}
void DownloadingListWidget::updateItem(const std::shared_ptr<aria2net::AriaTellStatus>& status)
{
    if (const auto& r = status->result; !r.gid.empty())
    {
        const auto item = qobject_cast<DownloadingItemWidget*>(itemWidget(m_items[r.gid]));
        if (!r.errorCode.empty())
        {
            item->status = status;
            item->updateStatus();
        }
    }
}
void DownloadingListWidget::deleteItem(const std::string& gid)
{
    const int row = this->row(m_items[gid]);
    this->takeItem(row);
    this->update();
    m_items.erase(gid);
}
