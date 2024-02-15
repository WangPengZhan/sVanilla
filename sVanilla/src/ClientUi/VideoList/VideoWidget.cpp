#include "VideoWidget.h"
#include "ui_VideoWidget.h"
#include "VideoGridWidget.h"
#include "SUI/ToggleSwitch.h"
#include <QPushButton>
#include <QtWidgets/QLabel>

VideoWidget::VideoWidget(QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::VideoPage)
{
    ui->setupUi(this);
    detailAnimation = std::make_shared<GeometryAnimation>(this);
    detailAnimation->setTargetObject(detailPanel());
    ui->VideoDetailWidget->hide();
    ui->VideoStackedPage->setCurrentWidget(ui->VideoGrid);
    connect(ui->SwitchBtn, &ToggleSwitch::toggled, ui->VideoStackedPage, &QStackedWidget::setCurrentIndex);
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

void VideoWidget::updateVideoItem(const std::shared_ptr<Adapter::BaseVideoView>& videoView)
{
    ui->VideoGridWidget->addVideoItem(videoView->Identifier);
    ui->VideoListWidget->addVideoItem(videoView->Identifier);
    installBtnEventFilter();
    ui->VideoGridWidget->updateVideoItem(videoView);
    ui->VideoListWidget->updateVideoItem(videoView);
}

bool VideoWidget::processDetailsBtnClickEvent(QObject* watched)
{
    const auto clickedButton = qobject_cast<QPushButton*>(watched);
    if (!clickedButton)
    {
        return false;
    }

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

    if (!detailPanelVisible())
    {
        showDetailPanel();
        updateDetailPanelWidth();
    }
    else
    {
        hideDetailPanel();
    }
    if (const auto itemIdentifier = videoGridItemWidget->Identifier; detailSourceIdentifier != itemIdentifier)
    {
        //  可以在此处更新详情栏data
        detailSourceIdentifier = itemIdentifier;
        updateDetailPanel(videoGridItemWidget->m_videoView);
        return true;
    }
    return true;
}
bool VideoWidget::precessDownloadBtnClickEvent(QObject* watched)
{
    const auto clickedButton = qobject_cast<QPushButton*>(watched);
    if (!clickedButton)
    {
        return false;
    }
    if (clickedButton->objectName() != "VideoGridDownloadBtn")
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
    emit downloadBtnClick(videoGridItemWidget->m_videoView);
    return true;
}
void VideoWidget::installBtnEventFilter()
{
    auto btns = findChildren<QPushButton*>(Qt::FindChildrenRecursively);
    for (const auto btn : btns)
    {
        if (btn->objectName() == "VideoGridDetailsBtn" || btn->objectName() == "VideoGridDownloadBtn")
        {
            if (std::find(eventBtns.begin(), eventBtns.end(), btn) == eventBtns.end())
            {
                btn->installEventFilter(this);
            }
            else
            {
                eventBtns.push_back(btn);
            }
        }
    }
}

void VideoWidget::updateDetailPanel(const std::shared_ptr<Adapter::BaseVideoView>& videoView) const
{
    detailPanel()->updateUi(videoView);
}
VideoDetailWidget* VideoWidget::detailPanel() const
{
    return ui->VideoDetailWidget;
}
bool VideoWidget::detailPanelVisible() const
{
    return detailPanel()->isVisible();
}
void VideoWidget::showDetailPanel() const
{
    detailPanel()->show();
    // const auto rect = detailPanel()->geometry();
    // detailAnimation->setValues(rect.adjusted(detailPanel()->width(), 0, 0, 0), rect);
    // detailAnimation->start();
}
void VideoWidget::hideDetailPanel() const
{
    // const auto rect = detailPanel()->geometry();
    // detailAnimation->setValues(rect, rect.adjusted(detailPanel()->width(), 0, 0, 0));
    // connect(detailAnimation.get(), &GeometryAnimation::finish, detailPanel(), &QWidget::hide);
    // detailAnimation->start();
    detailPanel()->hide();
}
void VideoWidget::updateDetailPanelWidth()
{
    auto videoGridItemWidth = 350;
    auto remainingWidth = ui->VideoStackedPage->width() % videoGridItemWidth;
    // auto detailWidth = this->width() - remainingWidth;
    ui->VideoDetailWidget->setMinimumWidth(remainingWidth + detailPanel()->width());
    update();
}
bool VideoWidget::eventFilter(QObject* watched, QEvent* event)
{
    if (event->type() != QEvent::MouseButtonPress)
    {
        return false;
    }

    return processDetailsBtnClickEvent(watched) || precessDownloadBtnClickEvent(watched);
}
void VideoWidget::clearVideo()
{
    ui->VideoGridWidget->clearVideo();
    ui->VideoListWidget->clearVideo();
}
