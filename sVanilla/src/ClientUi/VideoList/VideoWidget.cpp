#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "VideoGridWidget.h"
#include <QtWidgets/QLabel>

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
{
    ui->setupUi(this);
    ui->VideoDetailWidget->hide();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);

    connect(ui->VideoListBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoList);
    });
    connect(ui->VideoGridBtn, &QPushButton::clicked, [this] {
        ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);
    });

    addVideoItem("123");
    addVideoItem("345");
    addVideoItem("567");
    auto VideoGridDetailsBtns = findChildren<QPushButton*>("VideoGridDetailsBtn", Qt::FindChildrenRecursively);
    for (const auto btn : VideoGridDetailsBtns)
    {
        btn->installEventFilter(this);
    }
    // connect(ui->VideoGridWidget, &VideoGridWidget::itemDetailBtnClick, this, &VideoWidget::showDetailPanel);
    // connect(ui->VideoListWidget, &VideoListWidget::itemDetailBtnClick,this,&VideoWidget::showDetailPanel);
    // connect(ui->VideoGridWidget, &VideoGridWidget::handleDetialCheckBtnClick, this, &VideoWidget::updateDetailPanelVisibility);
}

VideoWidget::~VideoWidget()
{
    delete ui;
}
void VideoWidget::addVideoItem(const std::string& bvid) const
{
    ui->VideoGridWidget->addVideoItem(bvid);
    ui->VideoListWidget->addVideoItem(bvid);
}

void VideoWidget::updateVideoItem(const std::shared_ptr<BiliApi::VideoView>& videoView) const
{
    ui->VideoGridWidget->addVideoItem(videoView->bvid);
    ui->VideoGridWidget->updateVideoItem(videoView);
}

bool VideoWidget::processDetailsBtnClickEvent(QObject* watched)
{
    const auto clickedButton = qobject_cast<QPushButton*>(watched);
    if (!clickedButton)
    {
        return false;
    }

    qDebug() << clickedButton->objectName();

    if (clickedButton->objectName() != "VideoGridDetailsBtn")
    {
        return false;
    }

    const auto videoGridItemWidgetObj = clickedButton->parentWidget()->parentWidget();
    if (!videoGridItemWidgetObj || videoGridItemWidgetObj->objectName() != "VideoGridItemWidget")
    {
        return false;
    }

    const auto videoGridItemWidget = qobject_cast<VideoGridItemWidget*>(videoGridItemWidgetObj);
    if (!videoGridItemWidget)
    {
        return false;
    }

    if (const auto itemIdentifier = videoGridItemWidget->Identifier; detailSourceIdentifier != itemIdentifier)
    {
        //  可以在此处更新详情栏data
        qDebug() << videoGridItemWidget->Identifier;
        detailSourceIdentifier = itemIdentifier;
        updateDetailPanel(videoGridItemWidget->Identifier);
        if (!detailPanelVisible())
        {
            showDetailPanel();
        }
    }
    else
    {
        hideDetailPanel();
    }
    return true;
}

void VideoWidget::updateDetailPanel(const std::string& detail) const
{
    ui->VideoDetailWidget->updateUi(detail);
}
bool VideoWidget::detailPanelVisible() const
{
    return ui->VideoDetailWidget->isVisible();
}
void VideoWidget::showDetailPanel() const
{
    ui->VideoDetailWidget->show();
}
void VideoWidget::hideDetailPanel() const
{
    ui->VideoDetailWidget->hide();
}
bool VideoWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::MouseButtonPress)
    {
        return false;
    }

    return processDetailsBtnClickEvent(watched);
}
